/// <summary>
/// Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Free Software
/// Foundation, Inc.
/// *
/// Author: Alexander Gnauck AG-Software, mailto:gnauck@ag-software.de
/// *
/// This file is part of GNU Libidn.
/// *
/// This library is free software; you can redistribute it and/or
/// modify it under the terms of the GNU Lesser General Public License
/// as published by the Free Software Foundation; either version 2.1 of
/// the License, or (at your option) any later version.
/// *
/// This library is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/// Lesser General Public License for more details.
/// *
/// You should have received a copy of the GNU Lesser General Public
/// License along with this library; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
/// USA
/// </summary>

using System;

namespace Gnu.Inet.Encoding
{		
	public class IDNAException : System.Exception
	{
		public static string CONTAINS_NON_LDH		= "Contains non-LDH characters.";
		public static string CONTAINS_HYPHEN		= "Leading or trailing hyphen not allowed.";
		public static string CONTAINS_ACE_PREFIX	= "ACE prefix (xn--) not allowed.";
		public static string TOO_LONG				= "String too long.";
		
		public IDNAException(string m) : base(m)
		{
		
		}

		// TODO
		public IDNAException(StringprepException e) :base( "", e)
		{
		}
		
		public IDNAException(PunycodeException e) : base( "", e)
		{
		}
	}
}