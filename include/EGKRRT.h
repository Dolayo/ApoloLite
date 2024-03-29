/**********************************************************************
 *
 * This code is part of the MRcore project
 * Author:   Daniel Olayo Andres
 * 
 * 
 * MRcore is licenced under the Common Creative License,
 * Attribution-NonCommercial-ShareAlike 3.0
 *
 * You are free:
 *   - to Share - to copy, distribute and transmit the work
 *   - to Remix - to adapt the work
 *
 * Under the following conditions:
 *   - Attribution. You must attribute the work in the manner specified
 *     by the author or licensor (but not in any way that suggests that
 *     they endorse you or your use of the work).
 *   - Noncommercial. You may not use this work for commercial purposes.
 *   - Share Alike. If you alter, transform, or build upon this work,
 *     you may distribute the resulting work only under the same or
 *     similar license to this one.
 *
 * Any of the above conditions can be waived if you get permission
 * from the copyright holder.  Nothing in this license impairs or
 * restricts the author's moral rights.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  
 **********************************************************************/

#ifndef __EGKRRT__H_
#define __EGKRRT__H_

#include <mrcore.h>
#include "rdtstar.h"

namespace mr{
	
class ShipState;

class EGKRobotPath : public RobotPath
{
public:

	EGKRobotPath():RobotPath(){}
	double getDistance();
	virtual void drawGL() override;
};

class EGKRRT: public RDTstar
{
protected:
	class EGKtree : public RDTstar::RDTtree
	{
	protected:

		class EGKpath : public RDTstar::RDTtree::PathSegment
		{
			// Methods and attributes from RDTstar just to keep in mind
			/*
			vector<RobotState*> _inter; 
			RobotState* _init;
			RobotState* _end;
			PathSegment* _parent;
			RobotState* operator[](int i) { return _inter[i]; }
			int size() { return (int)_inter.size(); }
			PathSegment() { _init = 0; _end = 0; _parent = 0; }
			*/
			protected: 

				class Circunference
				{
					public:
						Circunference(RobotState* ap_init, RobotState* ap_goal);
						Circunference() = delete;
						~Circunference() = default;

						std::pair <CurveZone, bool> StateZone(RobotState* ap_init) const;

						std::tuple<double, bool, bool> getRelativeAng(RobotState* ap_init) const;

						double getDistance(RobotState* ap_init) const;

						double getRadius() const {return _radius;}
						void setRadius(double& ar_r) { _radius = ar_r; }

						Vector2D getCenter() const { return _center; }
						void setCenter(Vector2D& ar_center) { _center = ar_center; }

						void drawGL();

					private:

						double _radius{ 0.0 };
						Vector2D _center{ 0, 0 };
						bool _b_is_Ok{true};
				};
				class Spline{

				public:
					Spline(RobotState* ap_init, RobotState* ap_goal, ZoneType& ar_zone);
					Spline() = delete;
					~Spline() = default;
					Vector2D Spfunction(double t);
					void CalculateDistance(RobotState* ap_init);
					std::pair<double, double> getDistanceTest(RobotState* ap_init);
					double QuadraticMin(Vector2D& pos, double& t1, double& t2, double& t3);
					double getDistance() const { return _distance; }
					double getDistanceT(Vector2D& pos, double& t);
					double getSqDistanceT(Vector2D& pos, double& t);
					double QuadraticPolynom(Vector2D& pos, double& t, double& t1, double& t2, double& t3);
					double Newton1(double& t, Vector2D& pos);
					double Newton2(double& t, Vector2D& pos);
					std::pair<CurveZone, bool> StateZone(RobotState* ap_init);
					bool IsInside(RobotState* ap_init);
					bool IsTNearLeft(RobotState* ap_init);
					std::tuple<double, bool, bool> getRelativeAng(RobotState* ap_init);
					Vector2D SpfirstD(double t);
					bool IsFeasible(ShipState* ap_init, ShipState* ap_goal);
					bool IsFreeSpace(RobotState* ap_init);
					void discretizeSpline();
					double getTnear(void) const { return _t_near; }
					bool getSTOP() const{ return _stop; }
					void setSTOP(bool ab_stop) { _stop = ab_stop; }
					void drawGL();
					bool IsOk() const { return _b_is_Ok; }
					std::vector<Vector2D> getDiscretizedPoints() { return _v_discretized_spline; }

					
					
				private:

					Vector2D _p0;//! Punto inicial
					Vector2D _p1;//! Punto de control 1
					Vector2D _p2;//! Punto de control 2
					Vector2D _p3;//! Punto final

					//! B(t) = _a.t3 + _b.t2 + _c.t + _p0;
					double _cx{ 0.0 };
					double _bx{ 0.0 };
					double _ax{ 0.0 };

					double _cy{ 0.0 };
					double _by{ 0.0 };
					double _ay{ 0.0 };

					bool _b_is_Ok{ true };

					double _t_near{ 0.0 };
					double _distance{ 0.0 };

					bool _stop{true};
					std::vector<double> ang_list;
					std::vector<Vector2D> _v_discretized_spline;
				};
			public:


				virtual ~EGKpath() 
				{
					if (_p_circ)
						delete _p_circ;
					/*if (_p_spline)
						delete _p_spline;*/
				}
				std::vector<std::vector<double>>_sequence;
				EGKpath():PathSegment(){}
				const EGKpath& operator=(const EGKpath& n) {
					_init = n._init;
					_end = n._end;
					_inter = n._inter;
					_parent = n._parent;
				}
				RobotState* operator[](int i) { return _inter[i]; }
				bool IsEqual(PathSegment* p_path);
				std::vector<Vector2D> getDiscretizedPoints() { return this->_p_spline->getDiscretizedPoints(); }
				double getLength();
				double getLength2States(RobotState* p_initState, RobotState* p_finalState);
				std::vector<RobotState*> getDiscretizedNeighbours(void) const {return _v_discretized_neighbours;}
				void discretizeNeighbours();
				EGKpath(const EGKpath& n){(*this) = n;}
				RobotState* last(){ return _inter.back(); }
				virtual void appendState(RobotState* p_aux){_inter.push_back(p_aux);}
				void appendCtrlAct(std::vector<double> v_aux) { _sequence.push_back(v_aux); }
				static EGKpath* createPath(RobotState* p_init, RobotState* p_end, bool& ar_success, int niter = 100, 
											bool b_ensure_yaw = false, bool b_precision = false);
				virtual std::vector<double> navigation(RobotState* p_initState, RobotState* p_finalState, 
														bool b_ensure_yaw = false, bool b_precision = false);
				static bool isGhostThere(ShipState* donkey, ShipState* carrot, Vector2D& newPos);
				bool generateCtrlActCirc(ShipState* ap_initState, Quadrant& ar_quad, ZoneType& ar_zone, std::vector<double>& ar_ctrl_act);
				bool generateCtrlActSpline(ShipState* ap_initState, Quadrant& ar_quad, ZoneType& ar_zone, std::vector<double>& ar_ctrl_act);
				bool IsLoop();
				bool isSplineOK()
				{
					if (_p_spline)
						return _p_spline->IsOk();
					else 
						return false;
				}
				void drawGL();

		private:
			Circunference* _p_circ{nullptr};
			std::shared_ptr<Spline> _p_spline{ nullptr };
			std::vector<RobotState*> _v_discretized_neighbours;
		};

	public:
		// Methods from RDTstar just to remember
		/*
		RobotState* _root;
		vector<PathSegment*> _paths;
		vector<RobotState*> _nodes;

		RDTtree() { _root = 0; _radius = 30; _divided = false; }
		int getNumNodes() { return (int)_nodes.size(); }
		virtual bool rootTree(RobotState* rot);
		virtual double distance(RobotState* p, PathSegment* path, RobotState** mnode = 0);
		virtual PathSegment* getClosestPathSegment(RobotState* n, RobotState** minstate);
		virtual RobotState* addNode(RobotState* n);
		virtual RobotPath getPathFromRoot(RobotState* n);
		virtual void Reconnect(vector<RobotState*>& v_nei, RobotState* Xnew);
		virtual PathSegment* findPath4Node(RobotState* node);
		virtual void getNeighbors(RobotState* Xnew, vector<RobotState*>* v_nei);
		virtual PathSegment* getBest(vector<RobotState*>& v_nei, RobotState** best);
		void drawGL();
	private:
		double _radius;
		bool _divided;
		void add(RobotState* n)
		{
			for (int i = 0; i < (int)_nodes.size(); i++)if (_nodes[i] == n)return;
			_nodes.push_back(n);
		}
		*/

		std::vector<RobotState*> _vertexes;

		std::vector<std::vector<double>> _sequence_solution;

		bool erase_vertex(RobotState* node)
		{
			bool b_ret = false;
			for (int i = 0; i < _vertexes.size(); ++i)
			{
				if (_vertexes[i]->isEqual(node))
				{
					_vertexes.erase(_vertexes.begin() + i);
					b_ret = true;
				}
			}
			return b_ret;
		}

		EGKtree() : RDTtree()
		{
		}
		//virtual double distance(RobotState* p, PathSegment* path, RobotState** mnode = 0) override;
		virtual RobotState* addNode(RobotState* node) override;
		virtual void Reconnect(vector<RobotState*>& v_nei, RobotState* Xnew, PathSegment* ap_initNodePath) override;
		virtual void getNeighbors(RobotState* Xnew, vector<RobotState*>* v_nei) override;
		void getLastNeighbors(RobotState* Xnew, vector<RobotState*>* v_nei);
		void getDiscretizedNeighbors(RobotState* Xnew, vector<RobotState*>* v_nei);
		//virtual double distance(RobotState* rs, PathSegment* path, RobotState** mnode = nullptr) override;
		//virtual PathSegment* getClosestPathSegment(RobotState* n, RobotState** minstate) override;
		virtual PathSegment* getBest(vector<RobotState*>& v_nei, RobotState** best) override;
		EGKRobotPath* GetPathFromRoot(ShipState* n);
		double distance(RobotState* p, PathSegment* path, RobotState** mnode) override;
		void PopulateVertexes();
		EGKRobotPath* findSolution(RobotState* goal);
		virtual PathSegment* findPath4Node(RobotState* node) override;
		PathSegment* findPath4NodeEnd(RobotState* node);
		virtual void drawGL() override;
		std::vector<double> _v_radius;
	};

public:
	
	/*RDTstar() :SBPathPlanner(), _treeStart(), _treeGoal()
	{
		_treeA = &_treeStart;
		_treeB = &_treeGoal;
	}
	virtual bool computePlan(int maxiterations);
	virtual bool setStartAndGoalStates(RobotState* start_, RobotState* goal_);
	int getNumNodes() { return _treeStart.getNumNodes() + _treeGoal.getNumNodes(); }*/

	virtual bool setStartAndGoalStates(RobotState* start_, RobotState* goal_) override;
	virtual bool computePlan(int maxiterations) override;
	bool AddMoreNodes(int maxiterations);
	bool testingPlan();
	EGKRRT():RDTstar()
	{
		_tree = new EGKtree;
	}
	virtual void drawGL() override;


protected:
	EGKtree* _tree{nullptr};
};


}



#endif  //__EGKRRT__H
