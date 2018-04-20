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
import sepolgen.policygen as policygen
import sepolgen.access as access
import sepolgen.refpolicy as refpolicy

class TestPolicyGenerator(unittest.TestCase):
    def setUp(self):
        self.g = policygen.PolicyGenerator()

    def test_init(self):
        self.assertFalse(self.g.xperms)

    def test_set_gen_xperms(self):
        self.g.set_gen_xperms(True)
        self.assertTrue(self.g.xperms)
        self.g.set_gen_xperms(False)
        self.assertFalse(self.g.xperms)

    def test_av_rules(self):
        av1 = access.AccessVector(["test_src_t", "test_tgt_t", "file", "ioctl"])
        av2 = access.AccessVector(["test_src_t", "test_tgt_t", "file", "open"])
        av3 = access.AccessVector(["test_src_t", "test_tgt_t", "file", "read"])

        avs = access.AccessVectorSet()
        avs.add_av(av1)
        avs.add_av(av2)
        avs.add_av(av3)

        self.g.add_access(avs)

        self.assertEqual(len(self.g.module.children), 1)
        r = self.g.module.children[0]
        self.assertIsInstance(r, refpolicy.AVRule)
        self.assertEqual(r.to_string(),
            "allow test_src_t test_tgt_t:file { ioctl open read };")

    def test_ext_av_rules(self):
        self.g.set_gen_xperms(True)

        av1 = access.AccessVector(["test_src_t", "test_tgt_t", "file", "ioctl"])
        av1.xperms['ioctl'] = refpolicy.XpermSet()
        av1.xperms['ioctl'].add(42)
        av2 = access.AccessVector(["test_src_t", "test_tgt_t", "file", "ioctl"])
        av2.xperms['ioctl'] = refpolicy.XpermSet()
        av2.xperms['ioctl'].add(1234)
        av3 = access.AccessVector(["test_src_t", "test_tgt_t", "dir", "ioctl"])
        av3.xperms['ioctl'] = refpolicy.XpermSet()
        av3.xperms['ioctl'].add(2345)

        avs = access.AccessVectorSet()
        avs.add_av(av1)
        avs.add_av(av2)
        avs.add_av(av3)

        self.g.add_access(avs)

        self.assertEqual(len(self.g.module.children), 4)

        # we cannot sort the rules, so find all rules manually
        av_rule1 = av_rule2 = av_ext_rule1 = av_ext_rule2 = None

        for r in self.g.module.children:
            if isinstance(r, refpolicy.AVRule):
                if 'file' in r.obj_classes:
                    av_rule1 = r
                else:
                    av_rule2 = r
            elif isinstance(r, refpolicy.AVExtRule):
                if 'file' in r.obj_classes:
                    av_ext_rule1 = r
                else:
                    av_ext_rule2 = r
            else:
                self.fail("Unexpected rule type '%s'" % type(r))

        # check that all rules are present
        self.assertNotIn(None, (av_rule1, av_rule2, av_ext_rule1, av_ext_rule2))

        self.assertEqual(av_rule1.rule_type, av_rule1.ALLOW)
        self.assertEqual(av_rule1.src_types, {"test_src_t"})
        self.assertEqual(av_rule1.tgt_types, {"test_tgt_t"})
        self.assertEqual(av_rule1.obj_classes, {"file"})
        self.assertEqual(av_rule1.perms, {"ioctl"})

        self.assertEqual(av_ext_rule1.rule_type, av_ext_rule1.ALLOWXPERM)
        self.assertEqual(av_ext_rule1.src_types, {"test_src_t"})
        self.assertEqual(av_ext_rule1.tgt_types, {"test_tgt_t"})
        self.assertEqual(av_ext_rule1.obj_classes, {"file"})
        self.assertEqual(av_ext_rule1.operation, "ioctl")
        xp1 = refpolicy.XpermSet()
        xp1.add(42)
        xp1.add(1234)
        self.assertEqual(av_ext_rule1.xperms.ranges, xp1.ranges)

        self.assertEqual(av_rule2.rule_type, av_rule2.ALLOW)
        self.assertEqual(av_rule2.src_types, {"test_src_t"})
        self.assertEqual(av_rule2.tgt_types, {"test_tgt_t"})
        self.assertEqual(av_rule2.obj_classes, {"dir"})
        self.assertEqual(av_rule2.perms, {"ioctl"})

        self.assertEqual(av_ext_rule2.rule_type, av_ext_rule2.ALLOWXPERM)
        self.assertEqual(av_ext_rule2.src_types, {"test_src_t"})
        self.assertEqual(av_ext_rule2.tgt_types, {"test_tgt_t"})
        self.assertEqual(av_ext_rule2.obj_classes, {"dir"})
        self.assertEqual(av_ext_rule2.operation, "ioctl")
        xp2 = refpolicy.XpermSet()
        xp2.add(2345)
        self.assertEqual(av_ext_rule2.xperms.ranges, xp2.ranges)

