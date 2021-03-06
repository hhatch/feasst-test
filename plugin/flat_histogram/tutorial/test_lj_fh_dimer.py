import sys
import unittest
import feasst
import pyfeasst
sys.path.insert(0, feasst.install_dir() + '/plugin/flat_histogram/py')
import lj_fh

class TestLJ_FH_benchmark(unittest.TestCase):
    def test(self):
        serial = False
        serial = True
        if serial:
            criteria = lj_fh.criteria_flathist(macro_min=0, macro_max=10, tmmc=True)
            mc2 = lj_fh.mc(criteria=criteria, forcefield='data.dimer')
            #print(criteria.write())

        else:
          # parallel
          #windows=[[0,2],[2,15],[15,25]]
          windows=feasst.window(0, 130, 8, 2)
          #print(windows)
          #windows=pyfeasst.vector_vector_to_list(feasst.window(0, 25, 3, 2))

          import multiprocessing as mp
          processes = [mp.Process(target=lj_fh.mc, args=(proc, windows[x][0], windows[x][1])) for x in range(len(windows))]
          #processes = [mp.Process(target=mc, args=(window[0], window[1])) for window in windows]

          for p in processes:
            p.start()

          for p in processes:
            p.join()

if __name__ == "__main__":
    unittest.main()
