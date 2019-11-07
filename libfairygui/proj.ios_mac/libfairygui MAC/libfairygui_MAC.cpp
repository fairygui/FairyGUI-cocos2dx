/*
 *  libfairygui_MAC.cpp
 *  libfairygui MAC
 *
 *  Created by ytom on 17/11/9.
 *
 *
 */

#include <iostream>
#include "libfairygui_MAC.hpp"
#include "libfairygui_MACPriv.hpp"

void libfairygui_MAC::HelloWorld(const char * s)
{
	 libfairygui_MACPriv *theObj = new libfairygui_MACPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void libfairygui_MACPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

