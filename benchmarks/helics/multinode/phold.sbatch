#!/bin/bash
#SBATCH -t 0:20:00
#SBATCH -p pdebug
#SBATCH -N 1

# the sbatch arguments set above can be changed by passing the same argument to the sbatch command (e.g. -N100 to use 100 nodes instead of 1)
# see the https://slurm.schedmd.com/ website for docs

# Give a way to override the name of the federate used... though things will likely break due to different benchmark federates using different arguments
if [[ "${BM_FED}" == "" ]]; then
    BM_FED="PholdFederate"
fi

# set coretype to other cores using --export=CORE_TYPE='<type>' argument to sbatch
if [[ "${CORE_TYPE}" == "" ]]; then
    CORE_TYPE="zmq"
fi
echo "Running ${BM_FED} benchmark on ${SLURM_JOB_NUM_NODES} nodes with core type ${CORE_TYPE}"

# set BUILD_PATH using sbatch argument --export=BUILD_PATH='<path>'
# default path is to a folder named build in the root of the git repository
if [[ "$BUILD_PATH" == "" ]]; then
    # if the build was in a git repository, use it as a hint for where the build folder could be located
    git_toplevel="$(git rev-parse --show-toplevel)"
    rv=$?
    if [[ "$rv" == "0" ]]; then
        BUILD_PATH="${git_toplevel}/build"
    else
        # guess at where the build path is
        BUILD_PATH="$PWD/../../../build"
    fi
fi

if [[ "${helics_broker_exe}" == "" ]]; then
    helics_broker_exe="${BUILD_PATH}/src/helics/apps/helics_broker"
fi
command -v "${helics_broker_exe}" || { echo >&2 "unable to find helics broker at ${helics_broker_exe}; check for working build and set BUILD_PATH if needed"; exit 1; }

if [[ "${helics_bm_fed_exe}" == "" ]]; then
    helics_bm_fed_exe="${BUILD_PATH}/benchmarks/helics/${BM_FED}"
fi
command -v "${helics_bm_fed_exe}" || { echo >&2 "unable to find benchmark federate at ${helics_bm_fed_exe}; check for working build and set BUILD_PATH if needed"; exit 1; }

# create an output folder for results
output_folder="${BM_FED}-${CORE_TYPE}-N${SLURM_JOB_NUM_NODES}-job-${SLURM_JOB_ID}"
mkdir -p "${output_folder}"

# Run info and srun job launch
echo "Running broker on $HOSTNAME"
"${BUILD_PATH}/src/helics/apps/helics_broker" --all -f "${SLURM_JOB_NUM_NODES}" --coretype "${CORE_TYPE}" --loglevel=5 > "${output_folder}/helics-broker-out.txt" 2>&1 &

# give some time for the broker to start up
sleep 5

# Run on 1 and 2 nodes to verify output, then scale to 4,8,16,32 to get performance numbers
# Run on 1 node with 2, 4, 8, 16, and 32 instances to verify Eaton results for single node
for i in $(seq 1 "${SLURM_JOB_NUM_NODES}");
do
    (( idx = i-1 ))
    srun -N1 "${helics_bm_fed_exe}" --index="$idx" --max_index="${SLURM_JOB_NUM_NODES}" --broker="${HOSTNAME}" --coretype "${CORE_TYPE}" > "${output_folder}/${BM_FED}-${idx}-out.txt" 2>&1 &
done

wait
echo 'Done'
