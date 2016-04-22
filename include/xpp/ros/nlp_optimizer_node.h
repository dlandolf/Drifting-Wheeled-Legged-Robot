/*
 * xpp_optimizer_node.h
 *
 *  Created on: Apr 21, 2016
 *      Author: winklera
 */

#ifndef USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_ROS_NLP_OPTIMIZER_NODE_H_
#define USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_ROS_NLP_OPTIMIZER_NODE_H_

#include <xpp/ros/optimizer_node_base.h>
#include <xpp/zmp/nlp_optimizer.h>

// ros services (.srv)
#include <xpp_opt/SolveNlp.h>



namespace xpp {
namespace ros {

class NlpOptimizerNode : public OptimizerNodeBase {
public:
  typedef xpp::utils::StdVecEigen2d StdVecEigen2d;
  typedef xpp::zmp::NlpOptimizer NlpOptimizer;

public:
  NlpOptimizerNode ();
  virtual ~NlpOptimizerNode () {};


private:

  void OptimizeTrajectory(VectorXd& opt_coefficients,
                          StdVecEigen2d& opt_footholds) const;
  std::vector<xpp::hyq::LegID> DetermineStepSequence() const;

  bool OptimizeTrajectoryService(xpp_opt::SolveNlp::Request& req,
                                 xpp_opt::SolveNlp::Response& res);


  ::ros::ServiceServer optimize_trajectory_srv_;
  ::ros::ServiceServer return_trajectory_srv_;

  NlpOptimizer nlp_optimizer_;
  StdVecEigen2d opt_footholds_;
};

} /* namespace ros */
} /* namespace xpp */

#endif /* USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_ROS_NLP_OPTIMIZER_NODE_H_ */
