#ifndef _TECHNOLOGY_H_
#define _TECHNOLOGY_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*! 
* \file technology.h
* \ingroup Objects
* \brief The technology class header file.
* \author Sonny Kim
*/

#include <string>
#include <vector>
#include <map>
#include <xercesc/dom/DOMNode.hpp>
#include "util/base/include/ivisitable.h"
#include "util/base/include/iround_trippable.h"

// Forward declaration
class Ghg;
class Emcoef_ind;
class GDP;
class DependencyFinder;
class IInfo;

/*! 
* \ingroup Objects
* \brief This technology class is based on the MiniCAM description of technology.
*
* The technology class is where all fuels are either consumed or transformed. The default technology class is based on a MiniCAM-style logit representation. This class has options for capacity limits, calibration, and fixed output technologies (for supply sectors) -- although these capabilities depend on interaction with the sub-sector and sector classes. 
*
* \author Sonny Kim
*/
class technology: public IVisitable, public IRoundTrippable
{
    friend class XMLDBOutputter;
private:
    static const std::string XML_NAME1D; //!< tag name for toInputXML
    static const std::string XML_NAME2D; //!< tag name for toInputXML
    void clear();
protected:
    std::string name; //!< technology name
    std::string fuelname; //!< name of fuel used
    int year; //!< period year or vintage
    double shrwts; //!< logit share weight
    double eff; //!< effective energy efficiency applies penalty to base 
    double effBase; //!< base energy efficiency read in
    double effPenalty; //!< energy efficiency penalty
    double intensity; //!< energy intensity (1/eff)
    double necost; //!< effective non-fuel costs applies penalty to base (levelized)
    double neCostBase; //!< base non-fuel costs read in(levelized)
    double neCostPenalty; //!< penalty on non-fuel costs 
    double fuelcost; //!< fuel cost only
    double techcost; //!< total cost of technology
    double tax; //!< utility tax
    double fMultiplier; //!< multiplier on fuel cost or price
    double pMultiplier; //!< multiplier on total cost or price
    double totalGHGCost; //!< the value of GHG tax + any storage cost, in $/GJ
    double fuelPrefElasticity; //!< Fuel preference elasticity
    double lexp; //!< logit exponential
    double share; //!< technology shares
    double input; //!< total fuel input (fossil and uranium)
    double output; //!< technology output
    double fixedOutput; //!< amount of fixed supply (>0) for this tech, exclusive of constraints
    double fixedOutputVal; //!< The actual fixed output value
    
    double calInputValue; // Calibration value
    double calOutputValue; // Calibration value
    bool doCalibration; // Flag set if calibration value is read-in
    bool doCalOutput; // Flag set if calibration value is read-in

    std::vector<Ghg*> ghg; //!< suite of greenhouse gases
    std::map<std::string,double> emissmap; //!< map of ghg emissions
    std::map<std::string,double> emfuelmap; //!< map of ghg emissions implicit in fuel
    std::map<std::string,double> emindmap; //!< map of indirect ghg emissions
    std::string note; //!< input data notation for this technology

    std::map<std::string,int> ghgNameMap; //!< Map of ghg name to integer position in vector. 
    virtual bool XMLDerivedClassParse( const std::string nodeName, const xercesc::DOMNode* curr );
    void calcTotalGHGCost( const std::string& regionName, const std::string& sectorName, const int per );
    virtual void toInputXMLDerived( std::ostream& out, Tabs* tabs ) const {};
    virtual void toDebugXMLDerived( const int period, std::ostream& out, Tabs* tabs ) const {};
    virtual void copy( const technology& techIn );
    void initElementalMembers();
    static double getFixedOutputDefault();
    virtual const std::string& getXMLName2D() const;
public:
    technology(); // default construtor
    technology( const technology& techIn ); // copy constructor.
    technology& operator=( const technology& techIn ); // assignment operator.
    virtual technology* clone() const;
    virtual ~technology();
    virtual void XMLParse( const xercesc::DOMNode* tempnode ); // initialize technology with xml data
     // for derived classes
    void completeInit( const std::string& aSectorName, DependencyFinder* aDepFinder );
    virtual void toInputXML( std::ostream& out, Tabs* tabs ) const;
    virtual void toDebugXML( const int period, std::ostream& out, Tabs* tabs ) const;
    virtual const std::string& getXMLName1D() const;
    static const std::string& getXMLNameStatic1D();
    static const std::string& getXMLNameStatic2D();
    virtual void initCalc( const IInfo* aSubsectorIInfo );
    virtual void calcCost( const std::string& regionName, const std::string& sectorName, const int per ); 
    virtual void calcShare( const std::string& regionName, const GDP* gdp, const int per ); 
    void normShare(double sum); // normalize technology share
    void resetFixedOutput(int per); // reset fixed supply to max value
    void adjShares(double subsecdmd, double subsecFixedOutput, double varShareTot, int per);
    void scaleFixedOutput(const double scaleRatio); // scale fixed supply
    // calculates fuel input and technology output
    virtual void production(const std::string& regionName,const std::string& prodName,double dmd, const GDP* gdp, const int per);
    virtual void indemission( const std::vector<Emcoef_ind>& emcoef_ind ); // calculates indirect GHG emissions from technology use
    virtual void calcEmission( const std::string& aGoodName, const int aPeriod ); // calculates GHG emissions from technology

    // ****** return names and values ******
    std::string getName() const; // return technology name
    std::string getFuelName() const; // return fuel name
    double getEff() const; // return fuel efficiency
    virtual double getIntensity(const int per) const; // return fuel intensity
    double getShare() const; // return normalized share
    bool getCalibrationStatus( ) const; // return true if technology has calibration value
    void scaleCalibrationInput( const double scaleFactor ); // scale calibration value
    void scaleShareWeight( double scaleValue );
    void setShareWeight( double shareWeightValue );
    double getCalibrationInput() const; // return calibration input value
    virtual double getCalibrationOutput() const; // return calibration output value
    virtual void adjustForCalibration( double subSectorDemand, const std::string& regionName, const IInfo* aSubsectorIInfo, const int period ); // Adjust share weights for calibration
    bool techAvailable( ) const; // Return available status (re: calibration)
    bool outputFixed() const; // return calibration output value
    double getInput() const; // return fuel input amount
    double getOutput() const; // return technology output
    double getFuelcost() const; // return fuel cost only
    double getTechcost() const; // return total technology cost
    double getNecost() const; // return non-fuel cost
    double getTotalGHGCost() const; // return carbon tax and storage cost added to tech in $/TC
    double getCarbonTaxPaid( const std::string& aRegionName, int aPeriod ) const;
    double getShareWeight() const;
    void technology::copyGHGParameters( const Ghg* prevGHG );
    Ghg* technology::getGHGPointer( const std::string& ghgName );
    const std::vector<std::string> getGHGNames() const;
    const std::map<std::string,double>& getemissmap() const; // return map of all ghg emissions
    const std::map<std::string,double>& getemfuelmap() const; // return map of all ghg emissions
    const std::map<std::string,double>& getemindmap() const; // return map of all ghg emissions
    double get_emissmap_second( const std::string& str ) const; // return value for ghg
    double getlexp() const; // return logit exponential for the technology
    double getFixedOutput() const; // return fixed output
    double getFixedInput() const; // return fixed input
    int getNumbGHGs()  const; // number of GHG objects in this technology
    void setYear( const int yearIn );
    void tabulateFixedDemands( const std::string regionName, const int period );
	void setTechShare(const double shareIn);
	virtual void accept( IVisitor* aVisitor, const int aPeriod ) const;
};
#endif // _TECHNOLOGY_H_
