/**
 * \file nepe2/config.h
 *
 * \brief Generated configuration file data for nepe2base.
 *
 * \copyright 2022 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#define MAKE_C_VERSION(X,Y) V ## X ## _ ## Y
#define NEPE2BASE_VERSION_SYM \
    MAKE_C_VERSION(@NEPE2BASE_VERSION_MAJOR@, @NEPE2BASE_VERSION_MINOR@)

#define NEPE2BASE_VERSION_STRING \
   "@NEPE2BASE_VERSION_MAJOR@.@NEPE2BASE_VERSION_MINOR@.@NEPE2BASE_VERSION_REL@"
