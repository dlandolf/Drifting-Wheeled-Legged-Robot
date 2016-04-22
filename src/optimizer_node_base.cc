/*
 * optimizer_node_base.cc
 *
 *  Created on: Apr 22, 2016
 *      Author: winklera
 */

#include <xpp/ros/optimizer_node_base.h>
#include <xpp/ros/ros_helpers.h>


namespace xpp {
namespace ros {

OptimizerNodeBase::OptimizerNodeBase ()
{
  ::ros::NodeHandle n_;
//  opt_var_pub_ = n_.advertise<xpp_opt::OptimizedVariables>("opt_variables",10);
  curr_state_sub_ = n_.subscribe("curr_state", 10,
                                &OptimizerNodeBase::CurrentStateCallback, this);
  goal_state_sub_ = n_.subscribe("goal_state", 10,
                                &OptimizerNodeBase::GoalStateCallback, this);

  return_coeff_srv_ = n_.advertiseService("return_optimized_coeff",
                                &OptimizerNodeBase::ReturnOptimizedCoeff, this);

  //FIXME get this from robot
  using namespace xpp::hyq;
  curr_stance_[LF] = Foothold( 0.35,  0.3, 0.0, LF);
  curr_stance_[RF] = Foothold( 0.35, -0.3, 0.0, RF);
  curr_stance_[LH] = Foothold(-0.35,  0.3, 0.0, LH);
  curr_stance_[RH] = Foothold(-0.35, -0.3, 0.0, RH);

  goal_cog_.p.x() = 0.5; // fixme, this should come from joystick
}

OptimizerNodeBase::~OptimizerNodeBase ()
{
  // TODO Auto-generated destructor stub
}


void
OptimizerNodeBase::CurrentStateCallback(const StateMsg& msg)
{
  curr_cog_ = RosHelpers::RosToXpp(msg);
}


void
OptimizerNodeBase::GoalStateCallback(const StateMsg& msg)
{
  goal_cog_ = RosHelpers::RosToXpp(msg);
}


bool
OptimizerNodeBase::ReturnOptimizedCoeff(xpp_opt::ReturnOptimizedCoeff::Request& req,
                                       xpp_opt::ReturnOptimizedCoeff::Response& res)
{
  res.coeff = xpp::ros::RosHelpers::XppToRos(opt_coefficients_);
  return true;
}


} /* namespace ros */
} /* namespace xpp */

