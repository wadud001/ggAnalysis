#!/usr/bin/env python

from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException

from datetime import date

def submit(config):
    try:
        crabCommand('submit', config = config)
    except HTTPException as hte:
        print "Failed submitting task: %s" % (hte.headers)
    except ClientException as cle:
        print "Failed submitting task: %s" % (cle)


workarea='/afs/hep.wisc.edu/user/wadud/private/CMSSW_10_6_24/src/ggAnalysis/ggNtuplizer/test/crab_submit//jobs//jobsUL2017v1Xsecs//ZLLGJetsMonoPhotonPtG130TuneCP513TeVamcatnloFXFXpythia8/'

mainOutputDir = '#mainOutputDir'

config                                = config()

config.General.requestName = '#jobname'
config.General.transferLogs = True
config.General.workArea = '%s' % workarea
config.General.transferOutputs = False
config.General.transferLogs = True

config.Site.storageSite = '#storageSite'
config.Site.whitelist = [#whitelist]
config.Site.blacklist = [#blacklist]


config.JobType.psetName  = '/afs/hep.wisc.edu/user/wadud/private/CMSSW_10_6_24/src/ggAnalysis/ggNtuplizer/test/ana.py'
config.JobType.pluginName  = 'Analysis'
# config.JobType.maxMemoryMB             = 2600
# config.JobType.maxJobRuntimeMin        = 5000

config.JobType.maxMemoryMB             = 1500
config.JobType.maxJobRuntimeMin        = 1000

config.Data.inputDBS = '#inputDBS'
config.Data.inputDataset = '#inputDataset'
config.Data.publication = False
config.Data.allowNonValidInputDataset = True
config.Data.outLFNDirBase = '%s' % mainOutputDir
config.Data.splitting     = '#splitting'
config.Data.unitsPerJob   = #unitsperjob
config.Data.totalUnits      =   100000
config.Data.ignoreLocality = True
config.Data.totalUnits = 50000000000
#config.Data.lumiMask = '#lumiMaskFile'

submit(config)