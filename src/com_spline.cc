/**
 @file    com_spline.cc
 @author  Alexander W. Winkler (winklera@ethz.ch)
 @date    Aug 23, 2016
 @brief   Brief description
 */

#include <xpp/zmp/com_spline.h>

namespace xpp {
namespace zmp {

static int kDim2d = xpp::utils::kDim2d;

ComSpline::ComSpline ()
{
  // TODO Auto-generated constructor stub
}

ComSpline::~ComSpline ()
{
  // TODO Auto-generated destructor stub
}

void
ComSpline::Init (int step_count, const SplineTimes& times,
                 bool insert_initial_stance)
{
  polynomials_.clear();

  // build the spline structure
  if (insert_initial_stance) {
    const int n_stance_splines = 2; // 3 allows quicker reaction
    double t = times.t_stance_initial_/n_stance_splines;
    for (int i=0; i<n_stance_splines; ++i)
      AddStancePolynomial(t);

//    double t_reaction = 0.06;
//    SplineContainer::AddStanceSpline(t_reaction);
//    SplineContainer::AddStanceSpline(times.t_stance_initial_-t_reaction);
  }

  AddPolynomialStepSequence(step_count, times.t_swing_);
}

void
ComSpline::AddPolynomialStepSequence (int step_count, double t_swing)
{
  unsigned int spline_id = polynomials_.empty() ? 0 : polynomials_.back().GetId()+1;
  int phase_id           = polynomials_.empty() ? 0 : polynomials_.back().phase_.id_+1;


  int n_splines_per_step = 1;
  for (int step=0; step<step_count; ++step) {
    for (int i=0; i<n_splines_per_step; ++i) {

      PhaseInfo phase(kStepPhase, step, phase_id);
      ComPolynomial spline(spline_id++, t_swing/n_splines_per_step, phase);
      spline.SetStep(step);
      polynomials_.push_back(spline);
    }
    phase_id++;
  }

  splines_initialized_ = true;
}

void
ComSpline::AddStancePolynomial (double t_stance)
{
  unsigned int spline_id = polynomials_.empty() ? 0 : polynomials_.back().GetId()+1;

  PhaseInfo last(kStancePhase, 0, 0);
  if (!polynomials_.empty()) {
    last = polynomials_.back().phase_;
    if (last.type_ != kStancePhase)
      last.id_++;
  }

  polynomials_.push_back(ComPolynomial(spline_id++, t_stance, last));

  splines_initialized_ = true;
}

double
ComSpline::GetTotalTime(const VecPolynomials& splines)
{
  double T = 0.0;
  for (const ComPolynomial& s: splines)
    T += s.GetDuration();
  return T;
}

int
ComSpline::Index (int spline, Coords dim, SplineCoeff coeff) const
{
  return GetFreeCoeffPerSpline() * kDim2d * spline + GetFreeCoeffPerSpline() * dim + coeff;
}

int
ComSpline::GetTotalFreeCoeff () const
{
  return polynomials_.size() * GetFreeCoeffPerSpline() * kDim2d;
}

int
ComSpline::GetPolynomialID(double t_global, const VecPolynomials& splines)
{
   assert(t_global<=GetTotalTime(splines));

   double t = 0;
   for (const ComPolynomial& s: splines) {
     t += s.GetDuration();

     if (t >= t_global) // at junctions, returns previous spline (=)
       return s.GetId();
   }
   assert(false); // this should never be reached
}

PhaseInfo
ComSpline::GetCurrentPhase (double t_global) const
{
  int id = GetPolynomialID(t_global);
  return polynomials_.at(id).phase_;
}

ComSpline::PhaseInfoVec
ComSpline::GetPhases () const
{
  PhaseInfoVec phases;

  int prev_id = -1;
  for (const auto& s : polynomials_) {
    PhaseInfo curr = s.phase_;
    if (curr.id_ != prev_id) { // never have the same phase twice
      phases.push_back(curr);
      prev_id = curr.id_;
    }
  }

  return phases;
}

double
ComSpline::GetLocalTime(double t_global, const VecPolynomials& splines)
{
  int id_spline = GetPolynomialID(t_global,splines);

  double t_local = t_global;
  for (int id=0; id<id_spline; id++) {
    t_local -= splines.at(id).GetDuration();
  }

  return t_local;//-eps_; // just to never get value greater than true duration due to rounding errors
}

ComSpline::Point2d
ComSpline::GetCOGxy(double t_global, const VecPolynomials& splines)
{
  int id = GetPolynomialID(t_global,splines);
  double t_local = GetLocalTime(t_global, splines);

  Point2d cog_xy;
  cog_xy.p = splines[id].GetState(xpp::utils::kPos, t_local);
  cog_xy.v = splines[id].GetState(xpp::utils::kVel, t_local);
  cog_xy.a = splines[id].GetState(xpp::utils::kAcc, t_local);

  return cog_xy;
}

ComSpline::VecScalar
ComSpline::ExpressComThroughCoeff (PosVelAcc posVelAcc, double t_local,
                                   int id, Coords dim) const
{
  switch (posVelAcc) {
    case xpp::utils::kPos:
      return ExpressCogPosThroughABCD(t_local, id, dim);
    case xpp::utils::kVel:
      return ExpressCogVelThroughABCD(t_local, id, dim);
    case xpp::utils::kAcc:
      return ExpressCogAccThroughABCD(t_local, id, dim);
    case xpp::utils::kJerk:
      return ExpressCogJerkThroughABCD(t_local, id, dim);
  }
}

void
ComSpline::CheckIfSplinesInitialized() const
{
  if (!splines_initialized_) {
    throw std::runtime_error("ComSpline.splines_ not initialized. Call Init() first");
  }
}


} /* namespace zmp */
} /* namespace xpp */
