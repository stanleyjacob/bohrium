/*
 * Copyright 2011 Troels Blum <troels@blum.dk>
 *
 * This file is part of cphVB.
 *
 * cphVB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cphVB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cphVB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PTXREGISTER_HPP
#define __PTXREGISTER_HPP

#include "PTXoperand.hpp"

enum RegSize
{
    _8,
    _16,
    _32,
    _64
};

#define PTX_SIZES (4)

class PTXregister : public PTXoperand
{
    PTXtype type;
    RegSize size;
    char* name;
public:
    char* asString();
    friend class PTXregisterBank;
};

#endif
