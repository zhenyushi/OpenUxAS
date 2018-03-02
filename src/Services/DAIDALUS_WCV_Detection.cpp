// ===============================================================================
// Authors: AFRL/RQQA
// Organization: Air Force Research Laboratory, Aerospace Systems Directorate, Power and Control Division
// 
// Copyright (c) 2017 Government of the United State of America, as represented by
// the Secretary of the Air Force.  No copyright is claimed in the United States under
// Title 17, U.S. Code.  All Other Rights Reserved.
// ===============================================================================

/* 
 * File:   DAIDALUS_WCV_Detection.cpp
 * Author: SeanR
 *
 *
 *
 * <Service Type="DAIDALUS_WCV_Detection" OptionString="Option_01" OptionInt="36" />
 * 
 */

// include header for this service
#include "DAIDALUS_WCV_Detection.h"
#include "Daidalus.h"
#include "Position.h"

//include for KeyValuePair LMCP Message
#include "afrl/cmasi/KeyValuePair.h" //this is an exemplar
#include "afrl/cmasi/AirVehicleState.h"

#include <iostream>     // std::cout, cerr, etc
#include <cmath>    //cmath::cos, sin, etc

// convenience definitions for the option strings
#define STRING_XML_OPTION_STRING "OptionString"
#define STRING_XML_OPTION_INT "OptionInt"

// useful definitions
#ifndef M_PI 
#define M_PI 3.141592653589793238462643
#endif

namespace {
    void makeVelocityXYZ(double u, double v, double w, double Phi, double Theta, double Psi, double& velocityX, double& velocityY, double& velocityZ)
    {
        velocityX = std::cos(Theta)*std::cos(Psi)*u + (std::sin(Phi)*std::sin(Theta)*std::cos(Psi)-std::cos(Phi)*std::sin(Psi))*v + std::sin(Phi)*std::sin(Psi);
        velocityY = std::cos(Theta)*std::sin(Psi)*u + (std::sin(Phi)*std::sin(Theta)*std::sin(Psi)+std::cos(Phi)*std::cos(Psi))*v + (std::cos(Phi)*std::sin(Theta)*std::sin(Psi)-std::sin(Phi)*std::cos(Psi))*w;
        velocityZ = -std::sin(Theta)*u + std::sin(Phi)*std::cos(Theta)*v + std::cos(Phi)*std::cos(Theta)*w;
    }
}

// namespace definitions
namespace uxas  // uxas::
{
namespace service   // uxas::service::
{
    const double PI = M_PI;
// this entry registers the service in the service creation registry
DAIDALUS_WCV_Detection::ServiceBase::CreationRegistrar<DAIDALUS_WCV_Detection>
DAIDALUS_WCV_Detection::s_registrar(DAIDALUS_WCV_Detection::s_registryServiceTypeNames());

//create a DAIDALUS object
larcfm::Daidalus daa;

// service constructor
DAIDALUS_WCV_Detection::DAIDALUS_WCV_Detection()
: ServiceBase(DAIDALUS_WCV_Detection::s_typeName(), DAIDALUS_WCV_Detection::s_directoryName()) { };

// service destructor
DAIDALUS_WCV_Detection::~DAIDALUS_WCV_Detection() { };

bool DAIDALUS_WCV_Detection::configure(const pugi::xml_node& ndComponent)
{
    bool isSuccess(true);

    // process options from the XML configuration node:
    if (!ndComponent.attribute(STRING_XML_OPTION_STRING).empty())
    {
        m_option01 = ndComponent.attribute(STRING_XML_OPTION_STRING).value();
    }
    if (!ndComponent.attribute(STRING_XML_OPTION_INT).empty())
    {
        m_option02 = ndComponent.attribute(STRING_XML_OPTION_INT).as_int();
    }

    // subscribe to messages::
    //addSubscriptionAddress(afrl::cmasi::KeyValuePair::Subscription);
    addSubscriptionAddress(afrl::cmasi::AirVehicleState::Subscription);
    std::cout << "Successfully subscribed to AirVehicleState from DAIDALUS_WCV_Detection." << std::endl;


    return (isSuccess);
}

bool DAIDALUS_WCV_Detection::initialize()
{
    // perform any required initialization before the service is started

    
    //std::cout << "*** INITIALIZING:: Service[" << s_typeName() << "] Service Id[" << m_serviceId << "] with working directory [" << m_workDirectoryName << "] *** " << std::endl;
    
    return (true);
}

bool DAIDALUS_WCV_Detection::start()
{
    // perform any actions required at the time the service starts
    //std::cout << "*** STARTING:: Service[" << s_typeName() << "] Service Id[" << m_serviceId << "] with working directory [" << m_workDirectoryName << "] *** " << std::endl;
    
    return (true);
};

bool DAIDALUS_WCV_Detection::terminate()
{
    // perform any action required during service termination, before destructor is called.
    std::cout << "*** TERMINATING:: Service[" << s_typeName() << "] Service Id[" << m_serviceId << "] with working directory [" << m_workDirectoryName << "] *** " << std::endl;
    
    return (true);
}

bool DAIDALUS_WCV_Detection::processReceivedLmcpMessage(std::unique_ptr<uxas::communications::data::LmcpMessage> receivedLmcpMessage)
{
    if (afrl::cmasi::isAirVehicleState(receivedLmcpMessage->m_object))
    {
        //receive message
        auto airVehicleState = std::static_pointer_cast<afrl::cmasi::AirVehicleState> (receivedLmcpMessage->m_object);
        //std::cout << "DAIDALUS_WCV_Detection has received an AirVehicleState" << std::endl ;
        //handle message
        auto Total_velocity = airVehicleState->getAirspeed();
        auto Total_velocity_calculated =std::sqrt(std::pow(airVehicleState->getU(),2)+std::pow(airVehicleState->getV(),2)+std::pow(airVehicleState->getW(),2));
        if (std::abs(Total_velocity-Total_velocity_calculated)>0.000001)
        {std::cout << "Danger!! Danger !!  Calculated velocity is not equivalent to broadcast velocity" << std::endl;
        std::cout << "Broadcast velocity = " << Total_velocity << " Calculated velocity = " << Total_velocity_calculated << std::endl;}
        
        //add air vehicle message state to the Daidalus Object
        auto s_temp = larcfm::Position::makeLatLonAlt(airVehicleState->getLocation()->getLatitude(), "deg",  airVehicleState->getLocation()->getLongitude(), "deg", airVehicleState->getLocation()->getAltitude(), "m") ;      
        auto u = airVehicleState->getU();
        auto v = airVehicleState->getV();
        auto w = airVehicleState->getW();
        auto Phi = airVehicleState->getRoll();
        auto Theta = airVehicleState->getPitch();
        auto Psi = airVehicleState->getHeading();
        double velocityX, velocityY, velocityZ;
        makeVelocityXYZ(u, v, w, Phi*PI/180.0, Theta*PI/180.0, Psi*PI/180.0, velocityX, velocityY, velocityZ);
        
      
        // send out response
        auto keyValuePairOut = std::make_shared<afrl::cmasi::KeyValuePair>();
        keyValuePairOut->setKey(s_typeName());
        keyValuePairOut->setValue(std::to_string(m_serviceId));
        sendSharedLmcpObjectBroadcastMessage(keyValuePairOut);
        
    }
    return false;
}

}; //namespace service
}; //namespace uxas