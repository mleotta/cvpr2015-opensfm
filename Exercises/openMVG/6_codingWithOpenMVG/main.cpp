
#include <iostream>
#include <fstream>

#include "openMVG/numeric/numeric.h"
#include "openMVG/sfm/sfm.hpp"
#include "third_party/cmdLine/cmdLine.h"

using namespace openMVG::sfm;

int main(int argc, char **argv)
{
  CmdLine cmd;

  std::string sSfM_Data_Filename;

  cmd.add( make_option('i', sSfM_Data_Filename, "input_file") );

  try {
      if (argc == 1) throw std::string("Invalid command line parameter.");
      cmd.process(argc, argv);
  } catch(const std::string& s) {
      std::cerr << "Display stats about a sfm_data.X file.\nUsage: " << argv[0] << "\n"
      << "[-i|--input_file file] path to a SfM_Data scene\n"
      << std::endl;

      std::cerr << s << std::endl;
      return EXIT_FAILURE;
  }

  //---------------------------------------
  // Read SfM Scene (image view names)
  //---------------------------------------
  SfM_Data sfm_data;
  if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(ALL))) {
    std::cerr << std::endl
      << "The input SfM_Data file \""<< sSfM_Data_Filename << "\" cannot be read." << std::endl;
    return EXIT_FAILURE;
  }

  // Display some stats about the loaded data:
  std::cout
    << "Loaded a sfm_data scene with:\n"
    << " #views: " << sfm_data.GetViews().size() << "\n"
    << " #poses: " << sfm_data.GetPoses().size() << "\n"
    << " #intrinsics: " << sfm_data.GetIntrinsics().size() <<  "\n"
    << " #tracks: " << sfm_data.GetLandmarks().size()
    << std::endl;

  
  return 0;
}
