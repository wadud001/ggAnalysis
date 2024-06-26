#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "ggAnalysis/ggNtuplizer/interface/ggNtuplizer.h"
#include "ggAnalysis/ggNtuplizer/interface/GenParticleParentage.h"
using namespace std;

enum PROMPT_STATUS_TYPE {
    DIRECTPROMPT, FRAGMENTPROMPT, LEPTONPROMPT, NOPROMPT, INVALID = -99
};

// (local) variables associated with tree branches
vector<Float_t>    pdf_;
Float_t            pthat_;
Float_t            genWeight_;
Float_t            genHT_;
Float_t            pdfWeight_;
vector<Float_t>    pdfSystWeight_;

Float_t          lheAQCD_;
Float_t          lheAQED_;
Float_t          lheScale_;
Int_t            lheIDprocess_;
Float_t          lheXWgt_;
Float_t          lhePDFpart1_;
Float_t          lhePDFpart2_;
vector<Int_t>    lheCol_;
vector<Int_t>    lheAntiCol_;
vector<Int_t>    lheMotherfirst_;
vector<Int_t>    lheMotherlast_;
vector<Float_t>  lhePx_;
vector<Float_t>  lhePy_;
vector<Float_t>  lhePz_;
vector<Float_t>  lheE_;
vector<Float_t>  lheM_;
vector<Float_t>  lheHel_;
vector<Float_t>  lheInvT_;


vector<Int_t>    lhePID_;
vector<Float_t>  lhePt_;
vector<Float_t>  lheEta_;
vector<Float_t>  lhePhi_;
vector<Short_t>  lheStatus_;
Float_t          alphaQED_;
Float_t          alphaQCD_;
vector<Float_t>  lheScales_;
Float_t          qScale_;
vector<Float_t>  genWeights_;

UChar_t          nPUInfo_;
UChar_t          nPU_;
UChar_t          puBX_;
UChar_t          puTrue_;

UShort_t         nMC_;
vector<Int_t>    mcPID;
vector<Float_t>  mcVtx;
vector<Float_t>  mcVty;
vector<Float_t>  mcVtz;
vector<Float_t>  mcPt;
vector<Float_t>  mcMass;
vector<Float_t>  mcEta;
vector<Float_t>  mcPhi;
vector<Float_t>  mcE;
vector<Float_t>  mcEt;
vector<Int_t>    mcGMomPID;
vector<Int_t>    mcMomPID;
vector<Float_t>  mcMomPt;
vector<Float_t>  mcMomMass;
vector<Float_t>  mcMomEta;
vector<Float_t>  mcMomPhi;
vector<Short_t>  mcIndex;
vector<UShort_t> mcStatusFlag;
vector<UChar_t>  mcParentage;
vector<Char_t>   mcPromptStatusType_;
Char_t           mcHasDirectPromptPho_;
vector<Short_t>  mcStatus;
vector<Float_t>  mcCalIsoDR03;
vector<Float_t>  mcTrkIsoDR03;
vector<Float_t>  mcCalIsoDR04;
vector<Float_t>  mcTrkIsoDR04;


PROMPT_STATUS_TYPE getPromptStatus(const reco::GenParticle& p, const edm::Handle<vector<reco::GenParticle>>& particles) {
    if (p.status()==1 && p.numberOfMothers() && (std::abs(p.mother(0)->pdgId())<=22 || p.mother(0)->pdgId() == 2212)) {
        for (auto& genP : *particles) {
            auto absId = std::abs(genP.pdgId());
            if (genP.status()==23 && ROOT::Math::VectorUtil::DeltaR(p.p4(), genP.p4())<0.4) {
                if (absId==21 || absId<7) return FRAGMENTPROMPT;
                if (absId==11 || absId==13 || absId==15) return LEPTONPROMPT;
            }
        }
        return DIRECTPROMPT;
    }
    return NOPROMPT;
};


Float_t getGenCalIso(edm::Handle<reco::GenParticleCollection> handle,
                     reco::GenParticleCollection::const_iterator thisPart,
                     Float_t dRMax, bool removeMu, bool removeNu) {

    // Returns Et sum.
    Float_t etSum = 0;

    for (reco::GenParticleCollection::const_iterator p = handle->begin(); p != handle->end(); ++p) {
        if (p == thisPart) continue;
        if (p->status() != 1) continue;

        // has to come from the same collision
        if (thisPart->collisionId() != p->collisionId())
            continue;

        Int_t pdgCode = abs(p->pdgId());

        // skip muons/neutrinos, if requested
        if (removeMu && pdgCode == 13) continue;
        if (removeNu && (pdgCode == 12 || pdgCode == 14 || pdgCode == 16)) continue;

        // pass a minimum Et threshold
        // if (p->et() < 0) continue;

        // must be within deltaR cone
        Float_t dR = reco::deltaR(thisPart->momentum(), p->momentum());
        if (dR > dRMax) continue;

        etSum += p->et();
    }

    return etSum;
}

Float_t getGenTrkIso(edm::Handle<reco::GenParticleCollection> handle,
                     reco::GenParticleCollection::const_iterator thisPart, Float_t dRMax) {

    // Returns pT sum without counting neutral particles.
    Float_t ptSum = 0;

    for (reco::GenParticleCollection::const_iterator p = handle->begin(); p != handle->end(); ++p) {
        if (p == thisPart) continue;
        if (p->status() != 1) continue;
        if (p->charge() == 0) continue;  // do not count neutral particles

        // has to come from the same collision
        if (thisPart->collisionId() != p->collisionId())
            continue;

        // pass a minimum pt threshold
        // if (p->pt() < 0) continue;

        // must be within deltaR cone
        Float_t dR = reco::deltaR(thisPart->momentum(), p->momentum());
        if (dR > dRMax) continue;

        ptSum += p->pt();
    }

    return ptSum;
}

void ggNtuplizer::branchesGenInfo(TTree* tree, edm::Service<TFileService> &fs) {

    tree->Branch("pdf",           &pdf_);
    tree->Branch("pthat",         &pthat_);
    tree->Branch("genWeight",     &genWeight_);
    tree->Branch("genHT",         &genHT_);

    if (dumpPDFSystWeight_) {
        tree->Branch("pdfWeight",     &pdfWeight_);
        tree->Branch("pdfSystWeight", &pdfSystWeight_);
    }

    tree->Branch("lheAQCD", &lheAQCD_);
    tree->Branch("lheAQED", &lheAQED_);
    tree->Branch("lheScale", &lheScale_);
    tree->Branch("lheIDprocess", &lheIDprocess_);
    tree->Branch("lheXWgt", &lheXWgt_);
    tree->Branch("lhePDFpart1", &lhePDFpart1_);
    tree->Branch("lhePDFpart2", &lhePDFpart2_);
    tree->Branch("lheCol", &lheCol_);
    tree->Branch("lheAntiCol", &lheAntiCol_);
    tree->Branch("lheMotherfirst", &lheMotherfirst_);
    tree->Branch("lheMotherlast", &lheMotherlast_);
    tree->Branch("lhePx", &lhePx_);
    tree->Branch("lhePy", &lhePy_);
    tree->Branch("lhePz", &lhePz_);
    tree->Branch("lheE", &lheE_);
    tree->Branch("lheM", &lheM_);
    tree->Branch("lheHel", &lheHel_);
    tree->Branch("lheInvT", &lheInvT_);

    tree->Branch("lhePID", &lhePID_);
    tree->Branch("lhePt", &lhePt_);
    tree->Branch("lheEta", &lheEta_);
    tree->Branch("lhePhi", &lhePhi_);
    tree->Branch("lheStatus", &lheStatus_);
    tree->Branch("lheScales", &lheScales_);

    tree->Branch("alphaQED", &alphaQED_);
    tree->Branch("alphaQCD", &alphaQCD_);
    tree->Branch("qScale", &qScale_);
    tree->Branch("genWeights", &genWeights_);

    tree->Branch("nPUInfo",       &nPUInfo_);
    tree->Branch("nPU",           &nPU_);
    tree->Branch("puBX",          &puBX_);
    tree->Branch("puTrue",        &puTrue_);

    hPU_        = fs->make<TH1F>("hPU",        "number of pileup",      1000,  0., 1000.);
    hPUTrue_    = fs->make<TH1F>("hPUTrue",    "number of true pileup", 1000, 0., 1000.);

    hPUw_ = fs->make<TH1F>("hPUw",        "number of pileup (weighted)",      1000,  0., 1000.);;
    hPUTruew_= fs->make<TH1F>("hPUTruew",    "number of true pileup  (weighted)", 1000, 0., 1000.);;

    hGenWeightSign_ = fs->make<TH1F>("hGenWeightSign", "Gen weight signs",           2,    -1, 1);
    hSumGenWeightSign_ = fs->make<TH1F>("hSumGenWeightSign", "Sum of Gen weight signs",1,  0, 1);
    hSumGenWeight_ = fs->make<TH1F>("hSumGenWeight", "Sum of Gen weights",1,  0, 1);
}

void ggNtuplizer::branchesGenPart(TTree* tree) {

    tree->Branch("nMC",          &nMC_);
    tree->Branch("mcPID",        &mcPID);
    tree->Branch("mcVtx",        &mcVtx);
    tree->Branch("mcVty",        &mcVty);
    tree->Branch("mcVtz",        &mcVtz);
    tree->Branch("mcPt",         &mcPt);
    tree->Branch("mcMass",       &mcMass);
    tree->Branch("mcEta",        &mcEta);
    tree->Branch("mcPhi",        &mcPhi);
    tree->Branch("mcE",          &mcE);
    tree->Branch("mcEt",         &mcEt);
    tree->Branch("mcGMomPID",    &mcGMomPID);
    tree->Branch("mcMomPID",     &mcMomPID);
    tree->Branch("mcMomPt",      &mcMomPt);
    tree->Branch("mcMomMass",    &mcMomMass);
    tree->Branch("mcMomEta",     &mcMomEta);
    tree->Branch("mcMomPhi",     &mcMomPhi);
    tree->Branch("mcIndex",      &mcIndex);
    tree->Branch("mcStatusFlag", &mcStatusFlag); //-999:non W or Z, 1:hardronic, 2:e, 3:mu, 4:tau
    tree->Branch("mcParentage",  &mcParentage);  // 16*lepton + 8*boson + 4*non-prompt + 2*qcd + exotics
    tree->Branch("mcStatus",     &mcStatus);     // status of the particle
    tree->Branch("mcPromptStatusType",  &mcPromptStatusType_);
    tree->Branch("mcHasDirectPromptPho",  &mcHasDirectPromptPho_);
    tree->Branch("mcCalIsoDR03", &mcCalIsoDR03);
    tree->Branch("mcTrkIsoDR03", &mcTrkIsoDR03);
    tree->Branch("mcCalIsoDR04", &mcCalIsoDR04);
    tree->Branch("mcTrkIsoDR04", &mcTrkIsoDR04);
}

void ggNtuplizer::fillGenInfo(const edm::Event& e) {

    // cleanup from previous execution

    lheAQCD_ = -999;
    lheAQED_ = -999;
    lheScale_ = -999;
    lheIDprocess_ = -999;
    lheXWgt_ = -999;
    lhePDFpart1_ = -999;
    lhePDFpart2_ = -999;
    lheCol_.clear();
    lheAntiCol_.clear();
    lheMotherfirst_.clear();
    lheMotherlast_.clear();
    lhePx_.clear();
    lhePy_.clear();
    lhePz_.clear();
    lheE_.clear();
    lheM_.clear();
    lheHel_.clear();
    lheInvT_.clear();

    lhePID_.clear();
    lhePt_.clear();
    lheEta_.clear();
    lhePhi_.clear();
    lheStatus_.clear();
    alphaQED_ = -999;
    alphaQCD_ = -999;
    lheScales_.clear();
    qScale_ = -999;
    genWeights_.clear();

    pthat_     = -99;;
    genWeight_ = -99;
    genHT_     = -99;
    nPUInfo_   = 0;
    pdfWeight_ = -99;
    pdf_          .clear();
    pdfSystWeight_.clear();

    edm::Handle<GenEventInfoProduct> genEventInfoHandle;
    e.getByToken(generatorLabel_, genEventInfoHandle);

    if (genEventInfoHandle.isValid()) {

        if (genEventInfoHandle->pdf()) {
            pdf_.push_back(genEventInfoHandle->pdf()->id.first);    // PDG ID of incoming parton #1
            pdf_.push_back(genEventInfoHandle->pdf()->id.second);   // PDG ID of incoming parton #2
            pdf_.push_back(genEventInfoHandle->pdf()->x.first);     // x value of parton #1
            pdf_.push_back(genEventInfoHandle->pdf()->x.second);    // x value of parton #2
            pdf_.push_back(genEventInfoHandle->pdf()->xPDF.first);  // PDF weight for parton #1
            pdf_.push_back(genEventInfoHandle->pdf()->xPDF.second); // PDF weight for parton #2
            pdf_.push_back(genEventInfoHandle->pdf()->scalePDF);    // scale of the hard interaction
        }

        if (genEventInfoHandle->hasBinningValues()) pthat_ = genEventInfoHandle->binningValues()[0];
        genWeight_ = genEventInfoHandle->weight();
        if (genWeight_ >= 0) hGenWeightSign_->Fill(0.5);
        else hGenWeightSign_->Fill(-0.5);
        if (abs(genWeight_)>1) hSumGenWeightSign_->Fill(0.5,genWeight_/abs(genWeight_));
        else hSumGenWeightSign_->Fill(0.5,genWeight_);
        hSumGenWeight_->Fill(0.5,genWeight_);

        qScale_ = genEventInfoHandle->qScale();
        alphaQED_ = genEventInfoHandle->alphaQED();
        alphaQCD_ = genEventInfoHandle->alphaQCD();

        for (UInt_t i =0; i < genEventInfoHandle->weights().size(); i++) {
            genWeights_.push_back(genEventInfoHandle->weights()[i]);
        }
    } else edm::LogWarning("ggNtuplizer") << "no GenEventInfoProduct in event";

    edm::Handle<LHEEventProduct> lheEventProduct;
    e.getByToken(lheEventLabel_, lheEventProduct);

    double lheHt   = 0.;
    if (lheEventProduct.isValid()) {
        ////    https://cmssdt.cern.ch/lxr/source/SimDataFormats/GeneratorProducts/interface/LesHouches.h
        const lhef::HEPEUP& lheEvent = lheEventProduct->hepeup();

        lheAQCD_ = lheEvent.AQCDUP;
        lheAQED_ = lheEvent.AQEDUP;
        lheScale_ = lheEvent.SCALUP;
        lheIDprocess_ = lheEvent.IDPRUP;
        lheXWgt_ = lheEvent.XWGTUP;
        lhePDFpart1_ = lheEvent.XPDWUP.first;
        lhePDFpart2_ = lheEvent.XPDWUP.second;

        std::vector<lhef::HEPEUP::FiveVector> lheParticles = lheEvent.PUP;
        size_t numParticles = lheParticles.size();

        for ( size_t idxParticle = 0; idxParticle < numParticles; ++idxParticle ) {

            lheCol_.push_back(lheEvent.ICOLUP[idxParticle].first);
            lheAntiCol_.push_back(lheEvent.ICOLUP[idxParticle].second);
            lheMotherfirst_.push_back(lheEvent.MOTHUP[idxParticle].first);
            lheMotherlast_.push_back(lheEvent.MOTHUP[idxParticle].second);
            lhePx_.push_back(lheParticles[idxParticle][0]);
            lhePy_.push_back(lheParticles[idxParticle][1]);
            lhePz_.push_back(lheParticles[idxParticle][2]);
            lheE_.push_back(lheParticles[idxParticle][3]);
            lheM_.push_back(lheParticles[idxParticle][4]);
            lheHel_.push_back(lheEvent.SPINUP[idxParticle]);
            lheInvT_.push_back(lheEvent.VTIMUP[idxParticle]);

            Int_t absPdgId = TMath::Abs(lheEvent.IDUP[idxParticle]);

            if (lheEvent.ISTUP[idxParticle] == 1 && ((absPdgId >= 1 && absPdgId <= 6) || absPdgId == 21) ) { // quarks and gluons
                lheHt += TMath::Sqrt(TMath::Power(lheParticles[idxParticle][0], 2.) + TMath::Power(lheParticles[idxParticle][1], 2.));
            }

            lhePID_.push_back(lheEvent.IDUP[idxParticle]);
            lhePt_.push_back(TMath::Sqrt(TMath::Power(lheParticles[idxParticle][0], 2.) + TMath::Power(lheParticles[idxParticle][1], 2.)));
            lheEta_.push_back(TMath::ATanH(lheParticles[idxParticle][2]/TMath::Sqrt(TMath::Power(lheParticles[idxParticle][0], 2.) + TMath::Power(lheParticles[idxParticle][1], 2.) + TMath::Power(lheParticles[idxParticle][2], 2.))));
            lhePhi_.push_back(TMath::ATan2(lheParticles[idxParticle][1], lheParticles[idxParticle][0]));
            lheStatus_.push_back(lheEvent.ISTUP[idxParticle]);
        }

        for (UInt_t i =0; i < lheEventProduct->scales().size(); i++) {
            lheScales_.push_back(lheEventProduct->scales()[i]);
        }

        if (dumpPDFSystWeight_) {
            pdfWeight_ = lheEventProduct->originalXWGTUP(); // PDF weight of this event !
            for (unsigned i = 0; i < lheEventProduct->weights().size(); ++i) {
                pdfSystWeight_.push_back(lheEventProduct->weights()[i].wgt);
            }
        }
    }

    genHT_   = lheHt;

    edm::Handle<vector<PileupSummaryInfo> > genPileupHandle;
    e.getByToken(puCollection_, genPileupHandle);

    if (genPileupHandle.isValid()) {
        for (vector<PileupSummaryInfo>::const_iterator pu = genPileupHandle->begin(); pu != genPileupHandle->end(); ++pu) {
            if (pu->getBunchCrossing() == 0) {
                hPU_->Fill(pu->getPU_NumInteractions() + 0.1);
                hPUTrue_->Fill(pu->getTrueNumInteractions() + 0.1);

                hPUw_->Fill(pu->getPU_NumInteractions() + 0.1, genWeight_);
                hPUTruew_->Fill(pu->getTrueNumInteractions() + 0.1, genWeight_);

                nPU_= pu->getPU_NumInteractions();
                puTrue_ = pu->getTrueNumInteractions();
                puBX_ = pu->getBunchCrossing();
            }

            nPUInfo_++;
        }
    } else edm::LogWarning("ggNtuplizer") << "no PileupSummaryInfo in event";

}

void ggNtuplizer::fillGenPart(const edm::Event& e) {

    // cleanup from previous execution
    mcPID       .clear();
    mcVtx       .clear();
    mcVty       .clear();
    mcVtz       .clear();
    mcPt        .clear();
    mcMass      .clear();
    mcEta       .clear();
    mcPhi       .clear();
    mcE         .clear();
    mcEt        .clear();
    mcGMomPID   .clear();
    mcMomPID    .clear();
    mcMomPt     .clear();
    mcMomMass   .clear();
    mcMomEta    .clear();
    mcMomPhi    .clear();
    mcIndex     .clear();
    mcStatusFlag.clear();
    mcParentage .clear();
    mcStatus    .clear();
    mcPromptStatusType_.clear();
    mcCalIsoDR03.clear();
    mcTrkIsoDR03.clear();
    mcCalIsoDR04.clear();
    mcTrkIsoDR04.clear();


    mcHasDirectPromptPho_ = 0;
    nMC_ = 0;

    edm::Handle<vector<reco::GenParticle> > genParticlesHandle;
    e.getByToken(genParticlesCollection_, genParticlesHandle);

    if (!genParticlesHandle.isValid()) {
        edm::LogWarning("ggNtuplizer") << "no reco::GenParticles in event";
        return;
    }

    Int_t genIndex = 0;

    for (vector<reco::GenParticle>::const_iterator ip = genParticlesHandle->begin(); ip != genParticlesHandle->end(); ++ip) {
        genIndex++;

        bool photonOrLepton =
            (ip->pdgId() == 22) ||
            (abs(ip->pdgId()) == 11) ||
            (abs(ip->pdgId()) == 13) ||
            ((abs(ip->pdgId()) == 12 || abs(ip->pdgId()) == 14 || abs(ip->pdgId()) == 16)) ||
            (abs(ip->pdgId()) >= 11 && abs(ip->pdgId()) <= 16)  ||
            (abs(ip->pdgId()) == 15);

        const reco::Candidate *p = (const reco::Candidate*)&(*ip);
        if (!runOnParticleGun_ && !p->mother()) continue;

        mcPID    .push_back(p->pdgId());
        mcVtx    .push_back(p->vx());
        mcVty    .push_back(p->vy());
        mcVtz    .push_back(p->vz());
        mcPt     .push_back(p->pt());
        mcMass   .push_back(p->mass());
        mcEta    .push_back(p->eta());
        mcPhi    .push_back(p->phi());
        mcE      .push_back(p->energy());
        mcEt     .push_back(p->et());
        mcStatus .push_back(p->status());


        UShort_t tmpStatusFlag = 0;
        if (ip->fromHardProcessFinalState()) setbit(tmpStatusFlag, 0);
        if (ip->isPromptFinalState())        setbit(tmpStatusFlag, 1);
        if (ip->isHardProcess())  setbit(tmpStatusFlag, 2);
        if (ip->isLastCopy())  setbit(tmpStatusFlag, 3);
        if (ip->fromHardProcessDecayed()) setbit(tmpStatusFlag, 4);
        if (ip->isLastCopyBeforeFSR()) setbit(tmpStatusFlag, 5);
        if (ip->fromHardProcessBeforeFSR()) setbit(tmpStatusFlag, 6);
        if (ip->isPromptDecayed()) setbit(tmpStatusFlag, 7);


        // if genParticle is W or Z, check its decay type
        if ( ip->pdgId() == 23 || abs(ip->pdgId()) == 24 ) {
            for (size_t k=0; k < p->numberOfDaughters(); ++k) {
                const reco::Candidate *dp = p->daughter(k);
                if (abs(dp->pdgId())<=6)                               setbit(tmpStatusFlag, 8);
                else if (abs(dp->pdgId())==11 || abs(dp->pdgId())==12) setbit(tmpStatusFlag, 9);
                else if (abs(dp->pdgId())==13 || abs(dp->pdgId())==14) setbit(tmpStatusFlag, 10);
                else if (abs(dp->pdgId())==15 || abs(dp->pdgId())==16) setbit(tmpStatusFlag, 11);
            }
        }

        mcStatusFlag.push_back(tmpStatusFlag);

        Char_t _mcPromptStatysType = getPromptStatus(*ip, genParticlesHandle);
        mcPromptStatusType_.push_back(_mcPromptStatysType);
        if ((ip->pdgId() == 22) && (_mcPromptStatysType == DIRECTPROMPT)) setbit(mcHasDirectPromptPho_, 0);
        if ((ip->pdgId() == 22) && (ip->isPromptFinalState())) setbit(mcHasDirectPromptPho_, 1);

        Int_t mcGMomPID_ = -999;
        Int_t mcMomPID_  = -999;
        Float_t mcMomPt_    = -999.;
        Float_t mcMomMass_  = -999.;
        Float_t mcMomEta_   = -999.;
        Float_t mcMomPhi_   = -999.;
        UChar_t temp_mcParentage = 0;
        if (!runOnSherpa_) {
            Int_t pIndex= std::distance(genParticlesHandle->begin(),ip);
            if (pIndex >= 0) {
                reco::GenParticleRef partRef = reco::GenParticleRef(genParticlesHandle,pIndex);
                genpartparentage::GenParticleParentage particleHistory(partRef);

                if (particleHistory.hasLeptonParent()) setbit(temp_mcParentage, 0);
                if (particleHistory.hasBosonParent()) setbit(temp_mcParentage, 1);
                if (particleHistory.hasNonPromptParent()) setbit(temp_mcParentage, 2);
                if (particleHistory.hasQCDParent()) setbit(temp_mcParentage, 3);
                if (particleHistory.hasExoticParent()) setbit(temp_mcParentage, 4);

                if ( particleHistory.hasRealParent() ) {
                    reco::GenParticleRef momRef = particleHistory.parent();
                    if ( momRef.isNonnull() && momRef.isAvailable() ) {
                        mcMomPID_  = momRef->pdgId();
                        mcMomPt_   = momRef->pt();
                        mcMomMass_ = momRef->mass();
                        mcMomEta_  = momRef->eta();
                        mcMomPhi_  = momRef->phi();

                        // get Granny
                        genpartparentage::GenParticleParentage motherParticle(momRef);
                        if ( motherParticle.hasRealParent() ) {
                            reco::GenParticleRef granny = motherParticle.parent();
                            mcGMomPID_ = granny->pdgId();
                        }
                    }
                }
            }
            mcParentage.push_back(temp_mcParentage);
            mcGMomPID.push_back(mcGMomPID_);
            mcMomPID.push_back(mcMomPID_);
            mcMomPt.push_back(mcMomPt_);
            mcMomMass.push_back(mcMomMass_);
            mcMomEta.push_back(mcMomEta_);
            mcMomPhi.push_back(mcMomPhi_);
        }

        mcIndex.push_back(genIndex-1);

        if (photonOrLepton) {
            mcCalIsoDR03.push_back( getGenCalIso(genParticlesHandle, ip, 0.3, false, false) );
            mcTrkIsoDR03.push_back( getGenTrkIso(genParticlesHandle, ip, 0.3) );
            mcCalIsoDR04.push_back( getGenCalIso(genParticlesHandle, ip, 0.4, false, false) );
            mcTrkIsoDR04.push_back( getGenTrkIso(genParticlesHandle, ip, 0.4) );
        } else {
            mcCalIsoDR03.push_back( -999. );
            mcTrkIsoDR03.push_back( -999. );
            mcCalIsoDR04.push_back( -999. );
            mcTrkIsoDR04.push_back( -999. );
        }

        nMC_++;
        // } // save info on particles of interest
    } // loop over gen-level particles

}
