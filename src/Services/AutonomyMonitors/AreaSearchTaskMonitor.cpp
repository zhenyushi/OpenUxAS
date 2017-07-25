#include "AutonomyMonitors/AreaSearchTaskMonitor.h"


namespace uxas {
namespace service {
namespace monitoring {
  
  AreaSearchTaskMonitor::AreaSearchTaskMonitor(AutonomyMonitorServiceMain * service_ptr, std::shared_ptr<afrl::cmasi::AreaSearchTask> pointSearchTask):MonitorBase(service_ptr), _task(pointSearchTask){};

  AreaSearchTaskMonitor::~AreaSearchTaskMonitor(){};

  void AreaSearchTaskMonitor::addVehicleStateMessage(VehicleStateMessage const & vMessage){
  }

  bool AreaSearchTaskMonitor::isPropertySatisfied(){
    return true;
  }

  double AreaSearchTaskMonitor::propertyRobustness(){
    return 0.0;
  }
  
}
}
}
