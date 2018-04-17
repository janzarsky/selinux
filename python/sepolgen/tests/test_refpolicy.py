# Authors: Karl MacMillan <kmacmillan@mentalrootkit.com>
#
# Copyright (C) 2006 Red Hat 
# see file 'COPYING' for use and warranty information
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; version 2 only
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

import unittest
import sepolgen.refpolicy as refpolicy
import selinux

class TestIdSet(unittest.TestCase):
    def test_set_to_str(self):
        s = refpolicy.IdSet(["read", "write", "getattr"])
        s = s.to_space_str().split(' ')
        s.sort()
        expected = "{ read write getattr }".split(' ')
        expected.sort()
        self.assertEqual(s, expected)
        s = refpolicy.IdSet()
        s.add("read")
        self.assertEqual(s.to_space_str(), "read")

class TestXpermSet(unittest.TestCase):
    def test_init(self):
        s1 = refpolicy.XpermSet()
        self.assertEqual(s1.complement, False)
        self.assertEqual(s1.ranges, [])
        
        s2 = refpolicy.XpermSet(True)
        self.assertEqual(s2.complement, True)
        self.assertEqual(s2.ranges, [])

    def test_normalize_ranges(self):
        s = refpolicy.XpermSet()
        s.ranges = [(1, 7), (5, 10), (100, 110), (102, 107), (200, 205),
            (205, 210), (300, 305), (306, 310), (400, 405), (407, 410),
            (500, 502), (504, 508), (500, 510)]
        s._XpermSet__normalize_ranges()

        i = 0
        r = list(sorted(s.ranges))
        while i < len(r) - 1:
            # check that range low bound is less than equal than the upper bound
            self.assertLessEqual(r[i][0], r[i][1])
            # check that two ranges are not overlapping or neighboring
            self.assertGreater(r[i + 1][0] - r[i][1], 1)
            i += 1

    def test_add(self):
        s = refpolicy.XpermSet()
        s.add(1, 7)
        s.add(5, 10)
        self.assertEqual(s.to_string(), "{ 1-10 }")

    def test_extend(self):
        a = refpolicy.XpermSet()
        a.add(1, 7)

        b = refpolicy.XpermSet()
        b.add(5, 10)

        a.extend(b)

        self.assertEqual(a.to_string(), "{ 1-10 }")

    def test_extend_complement(self):
        a = refpolicy.XpermSet(complement=True)
        a.add(1, 7)
        a.add(100, 110)
        a.add(200, 205)
        a.add(300, 305)
        a.add(400, 405)
        a.add(500, 502)
        a.add(504, 508)

        b = refpolicy.XpermSet(complement=True)
        b.add(5, 10)
        b.add(102, 107)
        b.add(205, 210)
        b.add(306, 310)
        b.add(407, 410)
        b.add(500, 510)

        a.extend(b)

        self.assertEqual(a.to_string(), "~ { 1-10 100-110 200-210 300-310 400-405 407-410 500-510 }")

    def test_extend_complement_2(self):
        a = refpolicy.XpermSet()
        a.add(1, 7)
        a.add(100, 110)
        a.add(200, 205)
        a.add(300, 305)
        a.add(400, 405)
        a.add(500, 502)
        a.add(504, 508)

        b = refpolicy.XpermSet(complement=True)
        b.add(5, 10)
        b.add(102, 107)
        b.add(205, 210)
        b.add(306, 310)
        b.add(407, 410)
        b.add(500, 510)

        a.extend(b)

        self.assertEqual(a.to_string(), "~ { 8-10 206-210 306-310 407-410 503 509-510 }")

    def test_extend_complement_3(self):
        a = refpolicy.XpermSet(complement=True)
        a.add(5, 10)
        a.add(102, 107)
        a.add(205, 210)
        a.add(306, 310)
        a.add(407, 410)
        a.add(500, 510)

        b = refpolicy.XpermSet()
        b.add(1, 7)
        b.add(100, 110)
        b.add(200, 205)
        b.add(300, 305)
        b.add(400, 405)
        b.add(500, 502)
        b.add(504, 508)

        a.extend(b)

        self.assertEqual(a.to_string(), "~ { 8-10 206-210 306-310 407-410 503 509-510 }")

class TestSecurityContext(unittest.TestCase):
    def test_init(self):
        sc = refpolicy.SecurityContext()
        sc = refpolicy.SecurityContext("user_u:object_r:foo_t")
    
    def test_from_string(self):
        context = "user_u:object_r:foo_t"
        sc = refpolicy.SecurityContext()
        sc.from_string(context)
        self.assertEqual(sc.user, "user_u")
        self.assertEqual(sc.role, "object_r")
        self.assertEqual(sc.type, "foo_t")
        self.assertEqual(sc.level, None)
        if selinux.is_selinux_mls_enabled():
            self.assertEqual(str(sc), context + ":s0")
        else:
            self.assertEqual(str(sc), context)
        self.assertEqual(sc.to_string(default_level="s1"), context + ":s1")

        context = "user_u:object_r:foo_t:s0-s0:c0-c255"
        sc = refpolicy.SecurityContext()
        sc.from_string(context)
        self.assertEqual(sc.user, "user_u")
        self.assertEqual(sc.role, "object_r")
        self.assertEqual(sc.type, "foo_t")
        self.assertEqual(sc.level, "s0-s0:c0-c255")
        self.assertEqual(str(sc), context)
        self.assertEqual(sc.to_string(), context)

        sc = refpolicy.SecurityContext()
        self.assertRaises(ValueError, sc.from_string, "abc")

    def test_equal(self):
        sc1 = refpolicy.SecurityContext("user_u:object_r:foo_t")
        sc2 = refpolicy.SecurityContext("user_u:object_r:foo_t")
        sc3 = refpolicy.SecurityContext("user_u:object_r:foo_t:s0")
        sc4 = refpolicy.SecurityContext("user_u:object_r:bar_t")

        self.assertEqual(sc1, sc2)
        self.assertNotEqual(sc1, sc3)
        self.assertNotEqual(sc1, sc4)

class TestObjecClass(unittest.TestCase):
    def test_init(self):
        o = refpolicy.ObjectClass(name="file")
        self.assertEqual(o.name, "file")
        self.assertTrue(isinstance(o.perms, set))

class TestAVRule(unittest.TestCase):
    def test_init(self):
        a = refpolicy.AVRule()
        self.assertEqual(a.rule_type, a.ALLOW)
        self.assertTrue(isinstance(a.src_types, set))
        self.assertTrue(isinstance(a.tgt_types, set))
        self.assertTrue(isinstance(a.obj_classes, set))
        self.assertTrue(isinstance(a.perms, set))

    def test_to_string(self):
        a = refpolicy.AVRule()
        a.src_types.add("foo_t")
        a.tgt_types.add("bar_t")
        a.obj_classes.add("file")
        a.perms.add("read")
        self.assertEqual(a.to_string(), "allow foo_t bar_t:file read;")

        a.rule_type = a.DONTAUDIT
        a.src_types.add("user_t")
        a.tgt_types.add("user_home_t")
        a.obj_classes.add("lnk_file")
        a.perms.add("write")
        # This test might need to go because set ordering is not guaranteed
        a = a.to_string().split(' ')
        a.sort()
        b = "dontaudit { foo_t user_t } { user_home_t bar_t }:{ lnk_file file } { read write };".split(' ')
        b.sort()
        self.assertEqual(a, b)

class TestTypeRule(unittest.TestCase):
    def test_init(self):
        a = refpolicy.TypeRule()
        self.assertEqual(a.rule_type, a.TYPE_TRANSITION)
        self.assertTrue(isinstance(a.src_types, set))
        self.assertTrue(isinstance(a.tgt_types, set))
        self.assertTrue(isinstance(a.obj_classes, set))
        self.assertEqual(a.dest_type, "")

    def test_to_string(self):
        a = refpolicy.TypeRule()
        a.src_types.add("foo_t")
        a.tgt_types.add("bar_exec_t")
        a.obj_classes.add("process")
        a.dest_type = "bar_t"
        self.assertEqual(a.to_string(), "type_transition foo_t bar_exec_t:process bar_t;")


class TestParseNode(unittest.TestCase):
    def test_walktree(self):
        # Construct a small tree
        h = refpolicy.Headers()
        a = refpolicy.AVRule()
        a.src_types.add("foo_t")
        a.tgt_types.add("bar_t")
        a.obj_classes.add("file")
        a.perms.add("read")

        ifcall = refpolicy.InterfaceCall(ifname="allow_foobar")
        ifcall.args.append("foo_t")
        ifcall.args.append("{ file dir }")

        i = refpolicy.Interface(name="foo")
        i.children.append(a)
        i.children.append(ifcall)
        h.children.append(i)

        a = refpolicy.AVRule()
        a.rule_type = a.DONTAUDIT
        a.src_types.add("user_t")
        a.tgt_types.add("user_home_t")
        a.obj_classes.add("lnk_file")
        a.perms.add("write")
        i = refpolicy.Interface(name="bar")
        i.children.append(a)
        h.children.append(i)

class TestHeaders(unittest.TestCase):
    def test_iter(self):
        h = refpolicy.Headers()
        h.children.append(refpolicy.Interface(name="foo"))
        h.children.append(refpolicy.Interface(name="bar"))
        h.children.append(refpolicy.ClassMap("file", "read write"))
        i = 0
        for node in h:
            i += 1
        self.assertEqual(i, 3)
        
        i = 0
        for node in h.interfaces():
            i += 1
        self.assertEqual(i, 2)
        
