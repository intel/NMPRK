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

/*******************************************************************************
***		 File		:	TransportPluginLoader.h								  **
***																			  **
********************************************************************************/
#ifndef _TRANSPORT_PLUGIN_LOADER_H
#define _TRANSPORT_PLUGIN_LOADER_H

#include <string>
#include <vector>
#include <map>

#include "TransportPluginDll.h"

/**
 * Transport Plugin Loader responsible for loading transport 
 * plug-in libraries and providing access to their implementations of the 
 * <code>transport_plugin_interface</code>.
 */
class TransportPluginLoader
{
public:
    /**
     * Dtor.
     */
    ~TransportPluginLoader();

    /**
     * Attempts to load specified transport plug-ins (dynamic libraries)
     * and retrieves entrypoints to their implementation of the
     * <code>transport_plugin_interface</code>.
     * 
     * @param pluginIds Array of plug-ins to load.
     *
     * @throws <code>std::runtime_error()</code> If either failed to load
     *      one of the dynamic libraries or failed to obtain entrypoints
     *      to the functions from the plug-in interface.
     */
    void load(const std::vector<PluginId>& pluginIds);

    /**
     * Retrieves a pointer to the specified plug-in interface.
     * 
     * @param id Id of the plug-in. Must be one of the Ids passed to
     *      the <code>load()</code>.
     *
     * @return Pointer to the plug-in interface.
     *
     * @throws <code>std::runtime_error()</code> If no plug-in
     *      with the specified <code>id</code> is loaded.
     */
    transport_plugin_interface* getPluginById(unsigned int id) const;

    /**
     * Retrieves a pointer to the specified plug-in interface.
     * 
     * @param name Name of the plug-in. Must be one of the names passed
     *      to the <code>load()</code>.
     *
     * @return Pointer to the plug-in interface.
     *
     * @throws <code>std::runtime_error()</code> If no plug-in
     *      with the specified <code>name</code> is loaded.
     */
    transport_plugin_interface* getPluginByName(const char* name) const;

protected:

    /**
     * Internal structure that holds all the information about
     * single plug-in loaded by the loader.
     */
    struct TransportPlugin
    {
        /// Id of the plug-in passed to <code>load()</code>.
        unsigned int id;

        /// Name of the plug-in passed to <code>load()</code>.
        std::string name;

        /// Pointer to the plug-in library.
        TransportPluginDll* pDll;

        /// Interace of the plug-in.
        transport_plugin_interface tpi;

        /// Contructs TransportPlugin: loads DLL, gets interace of the plug-in
        TransportPlugin(unsigned int pluginId, const char* pluginName);

        /// Destructor: unloads DLL
        ~TransportPlugin();

    };

    /// Array of loaded plugins.
    std::vector<TransportPlugin*> plugins;

    /// Maps <code>TransportPlugin::id</code> to the pointer
    /// for fast search by <code>getPluginById()</code>.
    std::map<unsigned int, TransportPlugin*> id2plugin;

    /// Maps <code>TransportPlugin::name</code> to the pointer
    /// for fast search by <code>getPluginByName()</code>.
    std::map<std::string, TransportPlugin*> name2plugin;

};

#endif //#ifndef _TRANSPORT_PLUGIN_LOADER_H
