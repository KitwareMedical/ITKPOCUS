import unittest
import itkpocus.util

DELTA = 0.00005


class TestUtil(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_get_framerate(self):
        x = { 'video' : { '@avg_frame_rate' : '5/20' } }
        f = itkpocus.util.get_framerate(x)
        self.assertAlmostEqual(f, 4, delta=DELTA)
    
if __name__ == '__main__':
    unittest.main()
