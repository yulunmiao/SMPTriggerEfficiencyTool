##Check Prompt Reco MiniAOD on das(https://cmsweb.cern.ch/das/) or using dasgoclient
dataset=/Muon0/Run2024*-PromptReco*/MINIAOD

2. Check config of dataset to grasp CMSSW version and Global Tag, do
cmsrel ${CMSSW-VERSION}
cp -r SMP/ ${CMSSW-VERSION}/src

3. Check https://cms-service-dqmdc.web.cern.ch/CAF/certification/ for golden json and copy into
${CMSSW-VERSION}/src/SMP/TriggerAnalyzer/test/
 
4. Compress CMMSW into tarball
tar -vcf ${CMSSW-VERSION}.tar ${CMSSW-VERSION}/ 

5. modify things in exe.sh
${CMSSW-VERSION}
process.GlobalTag = GlobalTag(process.GlobalTag, '${Global-Tag}', '')
process.source.lumisToProcess = ${GoldenJson}.getVLuminosityBlockRange()

6. Write jbl like below

Universe = vanilla
Executable = exe.sh

Requirements = OpSys == "LINUX" && (Arch != "DUMMY" )

request_disk = 10000000
request_memory = 6000

Should_Transfer_Files = YES
transfer_input_files = exe.sh, ${CMSSW-VERSION}.tar
notification = Never

Output = ./log/STDOUT_$(Cluster)$(Process).stdout
Error = ./log/STDERR_$(Cluster)$(Process).stderr
Log = ./log/LOG_$(Cluster)$(Process).log
x509userproxy = ${proxy}
+MaxRuntime           = 86400

Arguments= {file in dataset}
Queue

7. Submit job with
condor_submit 
