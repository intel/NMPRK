/***************************************************************************
 * Copyright 2012 Intel Corporation                                        *
 *Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 * http://www.apache.org/licenses/LICENSE-2.0                              *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/
/**************************************************************************
 * Author: Stewart Dale <IASI NM TEAM>                                    *
 * Updates:							                                      *
 * 4/30: prep for initial external release                                *
 **************************************************************************/


#include <sstream>
#include <stdexcept>

#include "TransportPluginLoader.h"
#include "Logger.h"
#include "transport.h"

using namespace std;

TransportPluginLoader::~TransportPluginLoader()
{
    for (size_t i = 0; i < plugins.size(); ++i)
    {
        delete plugins[i];
    }
}

TransportPluginLoader::TransportPlugin::~TransportPlugin()
{
    tpi.shutdown();
    delete this->pDll;
}

TransportPluginLoader::TransportPlugin::TransportPlugin(unsigned int pluginId,
                                                        const char* pluginName)
{
    this->name = pluginName;
    this->id = pluginId;

    try
    {
        this->pDll = new TransportPluginDll(pluginName);
        pDll->GetInterface(&this->tpi);

        dcmi_init_info_t pluginInitInfo;
        pluginInitInfo.loggerFun = ::TransportPlugin::Logger::log;
        tpi.initialize(&pluginInitInfo);
    } catch (exception& e)
    {
        throw runtime_error(string(pluginName) + " " + e.what());
    }
}

void
TransportPluginLoader::load(const std::vector<PluginId>& pluginIds)
{
    // do not try to load more than once
    if (!plugins.empty())
    {
        throw logic_error("Already loaded");
    }

    plugins.reserve(pluginIds.size());
    for (size_t i = 0; i < pluginIds.size(); ++i)
    {
        TransportPlugin* pPlugin = new TransportPlugin(pluginIds[i].id, pluginIds[i].name);

        // Add to the internal collections
        plugins.push_back(pPlugin);
        id2plugin[pPlugin->id] = plugins[plugins.size()-1];
        name2plugin[pPlugin->name] = plugins[plugins.size()-1];
    }
}

transport_plugin_interface*
TransportPluginLoader::getPluginById(unsigned int id) const
{
    map<unsigned int, TransportPlugin*>::const_iterator plugin = id2plugin.find(id);
    if (plugin == id2plugin.end())
    {
        throw runtime_error("Plugin not found");
    }
    return &plugin->second->tpi;
}

transport_plugin_interface*
TransportPluginLoader::getPluginByName(const char* name) const
{
    map<string, TransportPlugin*>::const_iterator plugin = name2plugin.find(name);
    if (plugin == name2plugin.end())
    {
        throw runtime_error("Plugin not found");
    }
    return &plugin->second->tpi;
}
