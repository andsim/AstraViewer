/** 
 * @file llplugininstance.h
 *
 * @cond
 * $LicenseInfo:firstyear=2008&license=viewergpl$
 * 
 * Copyright (c) 2008-2010, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlife.com/developers/opensource/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlife.com/developers/opensource/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 * 
 * @endcond
 */

#ifndef LL_LLPLUGININSTANCE_H
#define LL_LLPLUGININSTANCE_H

#include "llstring.h"

struct apr_dso_handle_t; //Cannot include llapr, as it defines NOUSER for windows, which breaks including commdlg.h!

/**
 * @brief LLPluginInstanceMessageListener receives messages sent from the plugin loader shell to the plugin.
 */
class LLPluginInstanceMessageListener
{
public:
	virtual ~LLPluginInstanceMessageListener();
   /** Plugin receives message from plugin loader shell. */
	virtual void receivePluginMessage(const std::string &message) = 0;
};

class BasicPluginBase;

/**
 * @brief LLPluginInstance handles loading the dynamic library of a plugin and setting up its entry points for message passing.
 */
class LLPluginInstance
{
	LOG_CLASS(LLPluginInstance);
public:
	LLPluginInstance(LLPluginInstanceMessageListener *owner);
	virtual ~LLPluginInstance();
	
	// Load a plugin dll/dylib/so
	// Returns 0 if successful, APR error code or error code returned from the plugin's init function on failure.
	int load(std::string &plugin_file);
	
	// Sends a message to the plugin.
	void sendMessage(const std::string &message);
	
   // TODO:DOC is this comment obsolete? can't find "send_count" anywhere in indra tree.
	// send_count is the maximum number of message to process from the send queue.  If negative, it will drain the queue completely.
	// The receive queue is always drained completely.
	// Returns the total number of messages processed from both queues.
	void idle(void);
	
	/** The signature of the function for sending a message from plugin to plugin loader shell.
    *
	 * @param[in] message_string Null-terminated C string 
    * @param[in] user_data The opaque reference that the callee supplied during setup.
    */
	typedef void (*receiveMessageFunction)(char const* message_string, BasicPluginBase** plugin_object);

	typedef void (*sendMessageFunction)(char const* message_string, LLPluginInstance** plugin_instance);

	/** The signature of the plugin init function. TODO:DOC check direction (pluging loader shell to plugin?)
    *
    * @param[in] host_user_data Data from plugin loader shell.
    * @param[in] plugin_send_function Function for sending from the plugin loader shell to plugin.
    */
	typedef int (*pluginInitFunction)(sendMessageFunction send_message_function, LLPluginInstance* plugin_instance, receiveMessageFunction* receive_message_function, BasicPluginBase** plugin_object);
	
   /** Name of plugin init function */
	static const char *PLUGIN_INIT_FUNCTION_NAME;
	
private:
	static void staticReceiveMessage(char const* message_string, LLPluginInstance** plugin_instance);
	void receiveMessage(const char *message_string);

	apr_dso_handle_t *mDSOHandle;
	
	BasicPluginBase* mPluginObject;
	receiveMessageFunction mReceiveMessageFunction;
	
	LLPluginInstanceMessageListener *mOwner;
};

#endif // LL_LLPLUGININSTANCE_H
