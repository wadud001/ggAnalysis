#ifndef ggNtuplizer_h
#define ggNtuplizer_h

#include "TTree.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/METReco/interface/BeamHaloSummary.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
//#include "EgammaAnalysis/ElectronTools/interface/EnergyScaleCorrection_class.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
//#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/PatCandidates/interface/IsolatedTrack.h"

#include "iterator"

using namespace std;

template <typename typeA, typename typeB>
void setbit(typeA & x, typeB bit) {
  typeA a = 1;
  x |= (a << bit);
}


class ggNtuplizer : public edm::EDAnalyzer {
 public:

  explicit ggNtuplizer(const edm::ParameterSet&);
  ~ggNtuplizer();

  //   static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

 private:

  //   virtual void beginJob() {};
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

  void initTriggerFilters(const edm::Event&);
  ULong64_t matchSingleElectronTriggerFilters(double pt, double eta, double phi);
  ULong64_t matchDoubleElectronTriggerFilters(double pt, double eta, double phi);
  ULong64_t matchSinglePhotonTriggerFilters(double pt, double eta, double phi);
  ULong64_t matchDoublePhotonTriggerFilters(double pt, double eta, double phi);
  ULong64_t matchTriplePhotonTriggerFilters(double pt, double eta, double phi);
  ULong64_t matchMuonTriggerFilters(double pt, double eta, double phi);
  ULong64_t matchJetTriggerFilters(double pt, double eta, double phi);
  ULong64_t matchL1TriggerFilters(double pt, double eta, double phi);
  Double_t deltaPhi(Double_t phi1, Double_t phi2);
  Double_t deltaR(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2);

  Short_t findSecondaryIndex(const Short_t & searchIndex, const std::vector<UShort_t> & container);

  void branchesTrigObj(TTree*);
  void branchesGlobalEvent(TTree*);
  void branchesGenInfo    (TTree*, edm::Service<TFileService>&);
  void branchesGenPart    (TTree*);
  void branchesMET        (TTree*);
  void branchesPhotons    (TTree*);
  void branchesPhotonsOOT(TTree*);
  void branchesElectrons  (TTree*);
  // void branchesHFElectrons(TTree*);
  void branchesMuons      (TTree*);
  void branchesAK4CHSJets       (TTree*);
  void branchesAK4PUPPIJets       (TTree*);
  void branchesAK8PUPPIJets    (TTree*);

  void fillGlobalEvent(const edm::Event&, const edm::EventSetup&);
  void fillGenInfo    (const edm::Event&);
  void fillGenPart    (const edm::Event&);
  void fillMET        (const edm::Event&, const edm::EventSetup&);
  void fillPhotons    (const edm::Event&, const edm::EventSetup&);
  void fillPhotonsOOT    (const edm::Event&, const edm::EventSetup&);
  void fillElectrons  (const edm::Event&, const edm::EventSetup&, math::XYZPoint&);
  void fillMuons      (const edm::Event&, math::XYZPoint&, const reco::Vertex);
  void fillAK8PUPPIJets(const edm::Event&, const edm::EventSetup&);
  void branchesGenAK8JetPart(TTree*);

  void fillAK4CHSJets       (const edm::Event&, const edm::EventSetup&);
  void fillAK4PUPPIJets       (const edm::Event&, const edm::EventSetup&);

  void branchesGenAK4JetPart(TTree*);
  void fillGenAK4JetInfo(const edm::Event&, Float_t );
  void fillGenAK8JetInfo(const edm::Event&, Float_t );

  const EcalRecHit * getECALrecHit(const DetId & id, noZS::EcalClusterLazyTools & ltNoZS, int di=0, int dj=0);
  Float_t ECALrecHitE(const DetId & id, noZS::EcalClusterLazyTools & ltNoZS, int di=0, int dj=0);
  Float_t ECALrecHitT(const DetId & id, noZS::EcalClusterLazyTools & ltNoZS);
  Float_t swissCross(const DetId& id, noZS::EcalClusterLazyTools & ltNoZS);

  std::vector<int> IndexMatchedConversion( edm::View<pat::Photon>::const_iterator g, const std::vector<edm::Ptr<reco::Conversion> > &, const std::vector<edm::Ptr<reco::Conversion> > &) const;
  double vtxZFromConv( edm::View<pat::Photon>::const_iterator pho, const edm::Ptr<reco::Conversion> &, const math::XYZPoint &beamSpot) const;
  double vtxZFromConvSuperCluster( edm::View<pat::Photon>::const_iterator pho, const edm::Ptr<reco::Conversion> &conversion, const math::XYZPoint &beamSpot ) const ;
  double vtxZFromConvOnly(  edm::View<pat::Photon>::const_iterator pho, const edm::Ptr<reco::Conversion> &conversion, const math::XYZPoint &beamSpot ) const;


  std::vector<Short_t> phoSCindex_;
  std::vector<Short_t> ootPho_SCindex_;
  std::vector<UShort_t> ecalSCindex_;
  std::vector<UShort_t> ecalootSCindex_;
  std::vector<Short_t> eleSCindex_;
  std::vector<Short_t> phoDirectEcalSCindex_;
  std::vector<Short_t> ootPhoDirectEcalSCindex_;
  std::vector<Short_t> eleDirectEcalSCindex_;

  void branchesECALSC(TTree* tree);
  void branchesECALOOTSC(TTree* tree);
  void branchesPhoECALSC(TTree* tree);
  void branchesEleECALSC(TTree* tree);
  void branchesootPhoECALSC(TTree* tree);
  void fillECALSC(const edm::Event& e, const edm::EventSetup& es);
  void fillECALOOTSC(const edm::Event& e, const edm::EventSetup& es);
  void resolvePhoECALSCindex();
  void resolveootPhoECALSCindex();
  void resolveEleECALSCindex();
  Float_t getLICTD(const reco::SuperCluster *sc, noZS::EcalClusterLazyTools & ltNoZS);
  Float_t etaWing(const DetId & id, noZS::EcalClusterLazyTools & ltNoZS);

  // void fillAK4PUPPIJets       (const edm::Event&, const edm::EventSetup&);

  void branchesTracks(TTree* tree);
  void fillTracks(const edm::Event& e, const edm::EventSetup& es);
  edm::EDGetTokenT<edm::View<pat::IsolatedTrack>>        tracksLabel_;


  void branchesTaus(TTree* tree);
  void fillTaus(const edm::Event& e, const edm::EventSetup& es);
  edm::EDGetTokenT<edm::View<pat::Tau>>        tauLabel_;


  Bool_t development_;
  Bool_t addFilterInfoAOD_;
  Bool_t addFilterInfoMINIAOD_;
  Bool_t doGenParticles_;
  Bool_t runOnParticleGun_;
  Bool_t doGenJets_;
  Bool_t runOnSherpa_;
  Bool_t dumpPFPhotons_;
  Bool_t dumpTaus_;
  Bool_t dumpJets_;
  Bool_t dumpAK8Jets_;
  Bool_t dumpSoftDrop_;
  Bool_t dumpPDFSystWeight_;
  Bool_t doOOTphotons_;
  Bool_t doTrks_;

  int  year_;

  vector<int> newparticles_;

  double trgFilterDeltaPtCut_;
  double trgFilterDeltaRCut_;


  edm::EDGetTokenT<reco::BeamHaloSummary>           beamHaloSummaryToken_;
  edm::EDGetTokenT<reco::VertexCollection>         vtxLabel_;
  edm::EDGetTokenT<reco::VertexCollection>         vtxBSLabel_;
  edm::EDGetTokenT<double>                         rhoLabel_;
  edm::EDGetTokenT<double>                         rhoCentralLabel_;
  edm::EDGetTokenT<trigger::TriggerEvent>          trgEventLabel_;
  edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjectsLabel_;
  edm::EDGetTokenT<edm::TriggerResults>            trgResultsLabel_;
  string                                           trgResultsProcess_;
  edm::EDGetTokenT<edm::TriggerResults>            patTrgResultsLabel_;
  edm::EDGetTokenT<GenEventInfoProduct>            generatorLabel_;
  edm::EDGetTokenT<LHEEventProduct>                lheEventLabel_;
  edm::EDGetTokenT<vector<PileupSummaryInfo> >     puCollection_;
  edm::EDGetTokenT<vector<reco::GenParticle> >     genParticlesCollection_;
  edm::EDGetTokenT<edm::View<pat::MET> >           pfMETlabel_;
  edm::EDGetTokenT<edm::View<pat::MET> >           puppiMETlabel_;
  edm::EDGetTokenT<edm::View<pat::Electron> >      electronCollection_;
  edm::EDGetTokenT<edm::View<pat::Photon> >        photonCollection_;
  edm::EDGetTokenT<edm::View<pat::Photon> >        photonOOTCollection_;
  edm::EDGetTokenT<edm::View<pat::Muon> >          muonCollection_;
  edm::EDGetTokenT<vector<pat::Tau> >              tauCollection_;
  edm::EDGetTokenT<EcalRecHitCollection>           ebReducedRecHitCollection_;
  edm::EDGetTokenT<EcalRecHitCollection>           eeReducedRecHitCollection_;
  edm::EDGetTokenT<EcalRecHitCollection>           esReducedRecHitCollection_;
  edm::EDGetTokenT<std::vector<reco::SuperCluster>>           ecalSCcollection_;
  edm::EDGetTokenT<std::vector<reco::SuperCluster>>           ecalSC_OOT_collection_;
  edm::EDGetTokenT<reco::PhotonCollection>         recophotonCollection_;
  edm::EDGetTokenT<reco::TrackCollection>          tracklabel_;
  edm::EDGetTokenT<reco::GsfElectronCollection>     gsfElectronlabel_;
  edm::EDGetTokenT<edm::View<reco::GsfTrack> >      gsfTracks_;
  edm::EDGetTokenT<reco::PFCandidateCollection>     pfAllParticles_;
  edm::EDGetTokenT<vector<pat::PackedCandidate> >   pckPFCdsLabel_;
  edm::EDGetTokenT<edm::View<reco::Candidate> >     recoCdsLabel_;
  edm::EDGetTokenT<edm::View<pat::Jet> >            ak4PFJetsCHSLabel_;
  edm::EDGetTokenT<edm::View<pat::Jet> >            ak4PFJetsPuppiLabel_;
  edm::EDGetTokenT<edm::View<pat::Jet> >            ak8JetsPUPPILabel_;
  edm::EDGetTokenT<std::vector<reco::GenJet> >      ak4PFJetsCHSGenJetLabel_;
  edm::EDGetTokenT<std::vector<reco::GenJet> >      ak8GenJetLabel_;
  edm::EDGetTokenT<reco::JetTagCollection>          boostedDoubleSVLabel_;
  edm::EDGetTokenT<pat::PackedCandidateCollection>  pckPFCandidateCollection_;
  edm::EDGetTokenT<Bool_t>                          ecalBadCalibFilterUpdateToken_;

  Bool_t                                            getECALprefiringWeights_;
  edm::EDGetTokenT<double>                          prefweight_token;
  edm::EDGetTokenT<double>                          prefweightup_token;
  edm::EDGetTokenT<double>                          prefweightdown_token;

  // for MET filters
  edm::EDGetTokenT<bool> BadChCandFilterToken_;
  edm::EDGetTokenT<bool> BadPFMuonFilterToken_;

  //check
  edm::EDGetToken gsfEle_;

  edm::EDGetTokenT<reco::BeamSpot> offlinebeamSpot_;
  edm::EDGetTokenT<edm::View<reco::Conversion> > convPhotonTag_;
  edm::EDGetTokenT<edm::View<reco::Conversion> > convPhotonTagSL_;

  double sigma1Pix;
  double sigma1Tib;
  double sigma1Tob;
  double sigma1PixFwd;
  double sigma1Tid;
  double sigma1Tec;
  double sigma2Pix;
  double sigma2Tib;
  double sigma2Tob;
  double sigma2PixFwd;
  double sigma2Tid;
  double sigma2Tec;
  double singlelegsigma1Pix;
  double singlelegsigma1Tib;
  double singlelegsigma1Tob;
  double singlelegsigma1PixFwd;
  double singlelegsigma1Tid;
  double singlelegsigma1Tec;
  double singlelegsigma2Pix;
  double singlelegsigma2Tib;
  double singlelegsigma2Tob;
  double singlelegsigma2PixFwd;
  double singlelegsigma2Tid;
  double singlelegsigma2Tec;

  TTree   *tree_;
  TH1F    *hEvents_;
  TH1F    *hPU_;
  TH1F    *hPUTrue_;
  TH1F    *hPUw_;
  TH1F    *hPUTruew_;
  TH1F    *hGenWeightSign_;
  TH1F    *hSumGenWeightSign_;
  TH1F    *hSumGenWeight_;

  HLTPrescaleProvider hltPrescaleProvider_;
};

#endif
