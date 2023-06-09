/*
 * Copyright (c) 2012, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
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

// key: compiler.err.prob.found.req
// key: compiler.misc.inconvertible.types
// key: compiler.misc.no.conforming.assignment.exists
// key: compiler.misc.cant.apply.symbol
// key: compiler.misc.invalid.mref
// key: compiler.misc.cant.apply.array.ctor
// key: compiler.misc.arg.length.mismatch

class CantApplySymbolFragment {

    interface SAM {
        void m(Integer u);
    }

    static void f(String s) { }

    void test() {
        SAM s = CantApplySymbolFragment::f;
    }

    void test2() {
        Runnable x = String[]::new;
    }
}
