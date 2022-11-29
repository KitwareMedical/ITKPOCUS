import unittest
import itkpocus.clarius
import numpy as np

SPACING_DELTA = 0.0005


class TestClarius(unittest.TestCase):
    # def setUp(self):
#         pass
    
#     def teardDown(self):
#         pass
    def test_load_and_preprocess_image(self):
        # consider using subTests in the future for different versions/images
        #  with self.subTest(i=i):
        #      self.assertEqual()...
        
        img, meta_dict = itkpocus.clarius.load_and_preprocess_image('./tests/data/clarius-ocular-image.png')
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        self.assertAlmostEqual(spacing[0], 0.2564, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.2564, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 77)
        self.assertAlmostEqual(crop[0,1], 882)
        self.assertAlmostEqual(np.max(img), 0.9490196)
        print(crop)
        print(np.max(img))
        
    def test_load_and_preprocess_video(self):
    # consider using subTests in the future for different versions/images
    #  with self.subTest(i=i):
    #      self.assertEqual()...

        img, meta_dict = itkpocus.clarius.load_and_preprocess_video('tests/data/clarius-ocular-short.mp4')
        spacing = meta_dict['spacing']
        crop = meta_dict['crop']
        self.assertAlmostEqual(spacing[0], 0.2564, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[1], 0.2564, delta=SPACING_DELTA)
        self.assertAlmostEqual(spacing[2], 0.04, delta=SPACING_DELTA)
    
        self.assertAlmostEqual(crop[0,0], 75)
        self.assertAlmostEqual(crop[0,1], 906)
        self.assertAlmostEqual(np.max(img), 1.0)
        print(crop)
        print(np.max(img))

if __name__ == '__main__':
    unittest.main()
    
    