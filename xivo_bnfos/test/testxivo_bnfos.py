# -*- coding: UTF-8 -*-

import xivo_bnfos
import unittest

class TestOverlayDict(unittest.TestCase):
    def setUp(self):
        self.d_base = {1: 1}
        self.d_overlay = xivo_bnfos.OverlayDict(self.d_base)
        
    def test_get_find_in_base_if_not_present(self):
        self.assertEqual(1, self.d_overlay[1])
    
    def test_set_only_affect_overlay(self):
        self.d_overlay[1] = 2
        self.assertEqual(1, self.d_base[1])
        
    def test_get_find_in_overlay_if_present(self):
        self.d_overlay[1] = 2
        self.assertEqual(2, self.d_overlay[1])

    def test_can_pass_arg_to_constructor(self):
        d = xivo_bnfos.OverlayDict({}, {1: 2})
        self.assertEqual(2, d[1])
    
    def test_len_is_sum_of_dict(self):
        self.d_overlay[2] = 3
        self.assertEqual(len(self.d_base) + 1, len(self.d_overlay))
        
    def test_in_will_find_in_base(self):
        self.assertTrue(1 in self.d_overlay)
        
    def test_in_will_find_in_overlay(self):
        self.d_overlay[2] = 3
        self.assertTrue(2 in self.d_overlay)
        
    def test_str(self):
        self.d_overlay[2] = 3
        self.assertEqual('{1: 2, 2: 3}', eval('str(self.d_overlay)'))
        
    def test_get(self):
        self.d_overlay[2] = 3
        self.assertEqual(1, self.d_overlay.get(1))
        self.assertEqual(3, self.d_overlay.get(2))
        self.assertEqual(None, self.d_overlay.get(3))
        
    def test_2stage_overlay_dict_works(self):
        self.d_overlay[2] = 3
        d_ovov = xivo_bnfos.OverlayDict(self.d_overlay, {3: 4})
        self.assertEqual(4, d_ovov[3])
        self.assertEqual(3, d_ovov[2])
        self.assertEqual(1, d_ovov[1])

