/*
 * Copyright (c) 2015, 2021, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

/*
 * ===========================================================================
 * (c) Copyright IBM Corp. 2023, 2023 All Rights Reserved
 * ===========================================================================
 */

package jdk.internal.access;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;

public interface JavaNetInetAddressAccess {
    /**
     * Return the original application specified hostname of
     * the given InetAddress object.
     */
    String getOriginalHostName(InetAddress ia);

    /**
     * Returns the 32-bit IPv4 address.
     */
    int addressValue(Inet4Address inet4Address);

    /**
     * Returns a reference to the byte[] with the IPv6 address.
     */
    byte[] addressBytes(Inet6Address inet6Address);

    /*[IF CRIU_SUPPORT]*/
    /**
     * To be invoked by CRIU post-restore hook, clear the cache.
     */
    void clearInetAddressCache();
    /*[ENDIF] CRIU_SUPPORT */
}
