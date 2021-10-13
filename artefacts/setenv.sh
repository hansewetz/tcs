# ------------------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------------------

# base paths to some places in this project
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
export ENV_ROOT=${DIR}
if [ $(basename ${DIR}) = env ]; then
  export PROJECT_ROOT=${DIR}/..
  TCS_DEPLOY_ROOT=${PROJECT_ROOT}/artefacts
else
  export PROJECT_ROOT=${DIR}
  TCS_DEPLOY_ROOT=${PROJECT_ROOT}
fi

# location of TCS binaries and libraries
TCS_BINDIR=${TCS_DEPLOY_ROOT}/bin
TCS_LIBDIR=${TCS_DEPLOY_ROOT}/bin


# option for gcc compiler (see: makerules/build.rules)      
export GCCDEBUG=-g                                                 
export GCC_STDOPT="c++20"                                   
export GCC_ERROR_IGNORE=""                                  
export GCC_USE_GLIBCXX11_ABI=""                             
export GCC_OPT="-Ofast -march=native"

# boost
export BOOST_INC=/share/local/src/boost_1_76_0/boost_1_76_0             
export BOOST_LIB=/share/local/src/boost_1_76_0/boost_1_76_0/stage/lib   

# first part of LD_LIBRARY_PATH
PREFIX_LD_LIBRARY_PATH=/usr/lib64

# amend paths
export PATH=/usr/bin:${TCS_BINDIR}:${SHAREDDRIVE}/bin:${PATH}
export LD_LIBRARY_PATH=${PREFIX_LD_LIBRARY_PATH}:${TCS_LIBDIR}:${LD_LIBRARY_PATH}:${BOOST_LIB}

# temp directory
export TMPDIR=/tmp

# setup dev environment (gcc etc.)                           
# (must be done after all other paths etc. have been setup)  
. scl_source enable devtoolset-10                             
