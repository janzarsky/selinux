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
import sepolgen.audit as audit
import selinux

class PolicyGenerator(unittest.TestCase):
    def setUp(self):
        # import selinux and replace matchpathcon function with this one
        def mock_matchpathcon(path, mode):
            return [0, 'system_u:object_r:test_t:s0']

        self.matchpathcon_bak = selinux.matchpathcon
        selinux.matchpathcon = mock_matchpathcon

    def test_check_mislabeled_nothing(self):
        """ Test AVC messages without mislabeled files. """
        g = policygen.PolicyGenerator()
        avs = access.AccessVectorSet()
        msg = audit.AVCMessage('')
        msg.path = '/test'
        avs.add('foo', 'test_t', 'file', ['getattr'], msg)

        g._PolicyGenerator__check_mislabeled(avs)

        for av in avs:
            self.assertEqual(av.mislabeled, set())

    def test_check_mislabeled_one(self):
        """ Test AVC messages with mislabeled files. """
        g = policygen.PolicyGenerator()
        avs = access.AccessVectorSet()
        msg = audit.AVCMessage('')
        msg.path = '/test'
        avs.add('foo', 'invalid_t', 'file', ['getattr'], msg)

        g._PolicyGenerator__check_mislabeled(avs)

        for av in avs:
            self.assertEqual(av.mislabeled, set(['/test']))

    def tearDown(self):
        selinux.matchpathcon = self.matchpathcon_bak
