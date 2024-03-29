#include "Ship.h"
#include <math.h>
#include "defines.h"

namespace mr
{

	Ship::Ship(double x, double y, double u, double v, double w) :
		_width(WIDTH),
		_length(LENGTH),
		_mass(MASS),
		_J(J),
		/*_xp(XP),
		_Sair(S_AIR),
		_Swater(S_WATER),*/
		_vMax(V_MAX),
		_aMax(A_MAX),
		//_Marm(M_ARM),
		//_trueWindDirection(TRUE_WIND_DIR),
		//_trueWaterDirection(TRUE_WATER_DIR),
		//_windSpeed(WIND_SPEED),//20
		//_waterSpeed(WATER_SPEED),//2
		//_Crs_water(CRS_WATER),
		//_Crs_air(CRS_AIR),
		//_ro_water(RO_WATER),
		//_ro_air(RO_AIR),
		_u(0),
		_v(0),
		_w(0),
		_x(0),
		_y(0),
		_yaw(0),
		_thrust_x(0),
		_thrust_y(0),
		/*_Wind_Force_Drag(0),
		_Water_Force_Drag(0),
		_Wind_Force_Side(0),
		_Water_Force_Side(0),
		_Wind_Moment(0),
		_Water_Moment(0),
		_Rotational_Moment(0),*/
		_move_success(true)

	{
		/*_alpha_water = _Crs_water * _Swater * _ro_water / (_length / 2);
		_alpha_air = _Crs_air * _Sair * _ro_air / (_length / 2);*/
		

		// Pintar el barquito
		vector<Vector2D> list_bod;
		PrismaticPart* hull = new PrismaticPart;
		double aux_width = _width / 5;
		double aux_length = _length / 5;
		list_bod.push_back(Vector2D(aux_width / 2, aux_width / 2));
		list_bod.push_back(Vector2D(aux_width / 4, -aux_width / 2));
		list_bod.push_back(Vector2D(-aux_width / 4, -aux_width / 2));
		list_bod.push_back(Vector2D(-aux_width / 2, aux_width / 2));
		hull->setPolygonalBase(list_bod);
		hull->setHeight(aux_length);
		hull->setRelativePosition(Vector3D(0, 0, aux_width / 2 + 0.001));
		hull->setRelativeOrientation(PI / 2, 0, PI / 2);
		hull->setColor(0.4, 0.4, 0.4);

		(*this) += hull;
		/*
		vector<Vector2D> list_head;
		Face tip;
		PrismaticPart* bow = new PrismaticPart;

		list_head.push_back(Vector2D(_width / 2, _width / 2));
		list_head.push_back(Vector2D(_width / 4, -_width / 2));
		list_head.push_back(Vector2D(-_width / 4, -_width / 2));
		list_head.push_back(Vector2D(-_width / 2, _width / 2));

		tip.addVertex(_width / 10, _width / 2);
		tip.addVertex(_width / 10, -((_width / 2) - (_width / 10)));
		tip.addVertex(-_width / 10, -((_width / 2) - (_width / 10)));
		tip.addVertex(-_width / 10, _width / 2);

		bow->setPolygonalBase(list_bod);
		bow->setPolygonalTop(tip);
		bow->setHeight(_length / 3);
		bow->setRelativePosition(Vector3D(0, 0, _width / 2));
		bow->setColor(0, 0, 1);

		(*this) += bow;
		*/
		vector<Vector2D> bridge_list;
		PrismaticPart* bridge = new PrismaticPart;

		bridge_list.push_back(Vector2D(aux_width / 2, aux_width / 2));
		bridge_list.push_back(Vector2D(aux_width / 4, -aux_width / 2));
		bridge_list.push_back(Vector2D(-aux_width / 4, -aux_width / 2));
		bridge_list.push_back(Vector2D(-aux_width / 2, aux_width / 2));
		bridge->setPolygonalBase(bridge_list);
		bridge->setHeight(aux_width);
		bridge->setRelativePosition(Vector3D(aux_length / 15, 0, aux_width));
		bridge->setRelativeOrientation(0, 0, PI / 2);
		bridge->setColor(0.4, 0.4, 0.4);

		(*this) += bridge;
	}

	void Ship::setState(double x, double y, double yaw, double u, double v, double w)
	{
		_x = x;
		_y = y;
		_yaw = yaw;

		_u = u;
		_v = v;
		_w = w;
	}

	void Ship::setState(Vector3D pos, Vector3D vel)
	{
		_x = pos.x;
		_y = pos.y;
		_yaw = pos.z;

		_u = vel.x;
		_v = vel.y;
		_w = vel.z;
	}

	Vector3D Ship::simpleAccs()
	{
		double ax = 0.0;
		double ay = 0.0;
		double aw = 0.0;

		double F_roz_x = 0.0;
		double F_roz_y = 0.0;
		double M_roz_w = 0.0;

		if ((_u > 0.001) || (_u < -0.001))
		{
			double sgn_vx = _u / std::abs(_u);
			F_roz_x = 0.6 * _mass * 9.18 * -sgn_vx;

		}

		if ((_v > 0.001) || (_v < -0.001))
		{
			double sgn_vy = _v / std::abs(_v);
			F_roz_y = 0.6 * _mass * 9.18 * -sgn_vy;

		}

		if ((_w > 0.001) || (_w < -0.001))
		{
			double sgn_w = _w / std::abs(_w);
			M_roz_w = 0.8 * _J * 9.18 * -sgn_w;
	
		}
		
		ax = F_roz_x/_mass;
		ay = F_roz_y / _mass;
		aw = M_roz_w / _J;

		if (std::abs(ax) > _aMax)
			ax = _aMax * (ax / std::abs(ax));

		if (std::abs(ay) > _aMax)
			ay = _aMax * (ay / std::abs(ay));

		if (std::abs(aw) > _aMax)
			aw = _aMax * (aw / std::abs(aw));

		return Vector3D(ax, ay, aw);
	}

	bool Ship::simpleDynamicsSim(double delta_t, bool b_use_collision)
	{
		double ax = 0.0;
		double ay = 0.0;
		double aw = 0.0;

		double F_roz_x = _mu_surge * _mass * 9.18;
		double F_roz_y = _mu_surge * _mass * 9.18;
		double M_roz_w = _mu_rot * _J * 9.18;

		if ((_u > 0.001) || (_u < -0.001))
		{
			double sgn_vx = _u / std::abs(_u);
			F_roz_x = F_roz_x * -sgn_vx;
		}

		if ((_v > 0.001) || (_v < -0.001))
		{
			double sgn_vy = _v / std::abs(_v);
			F_roz_y = F_roz_y * -sgn_vy;

		}

		if ((_w > 0.001) || (_w < -0.001))
		{
			double sgn_w = _w / std::abs(_w);
			M_roz_w = M_roz_w * -sgn_w;

		}


		double thrust_x_sum = 0.0;
		double thrust_y_sum = 0.0;
		double thrust_w_sum = 0.0;

		if ((_u > 0.001) || (_u < -0.001))
		{
			thrust_x_sum = _thrust_x + F_roz_x;
		}
		else
		{
			if (std::abs(_thrust_x) > std::abs(F_roz_x))
			{
				if(_thrust_x>0.0)
					thrust_x_sum = _thrust_x - F_roz_x;
				if(_thrust_x < 0.0)
					thrust_x_sum = _thrust_x + F_roz_x;
			}
				
		}

		if ((_v > 0.001) || (_v < -0.001))
		{
			thrust_y_sum = _thrust_y + F_roz_y;
		}
		else
		{
			if (std::abs(_thrust_y) > std::abs(F_roz_y))
			{
				if (_thrust_y > 0.0)
					thrust_y_sum = _thrust_y - F_roz_y;
				if (_thrust_y < 0.0)
					thrust_y_sum = _thrust_y + F_roz_y;
			}

		}

		if ((_w > 0.001) || (_w < -0.001))
		{
			thrust_w_sum = _thrust_w + M_roz_w;
		}
		else
		{
			if (std::abs(_thrust_w) > std::abs(M_roz_w))
			{
				if (_thrust_w > 0.0)
					thrust_w_sum = _thrust_w - M_roz_w;
				if (_thrust_w < 0.0)
					thrust_w_sum = _thrust_w + M_roz_w;
			}

		}

		ax = (thrust_x_sum) / _mass;
		ay = (thrust_y_sum) / _mass;
		aw = (thrust_w_sum) / _J;

		if (std::abs(ax) > _aMax)
			ax = _aMax * (ax / std::abs(ax));

		if (std::abs(ay) > _aMax)
			ay = _aMax * (ay / std::abs(ay));

		if (std::abs(aw) > _aMax)
			aw = _aMax * (aw / std::abs(aw));

		// MRUA
		double delta_x = _u * delta_t + 0.5 * ax * delta_t * delta_t;
		double delta_y = _v * delta_t + 0.5 * ay * delta_t * delta_t;

		// MCUA
		double delta_th = _w * delta_t + 0.5 * aw * delta_t * delta_t;
		_move_success = false;

		_u += ax * delta_t;

		if (std::abs(_u) > _vMax)
			_u = _vMax * (_u / std::abs(_u));

		_v += ay * delta_t;

		if (std::abs(_v) > _vMax)
			_v = _vMax * (_v / std::abs(_v));

		_w += aw * delta_t;

		if (std::abs(_w) > _vMax)
			_w = _vMax * (_w / std::abs(_w));

		//Transformation3D position = getAbsoluteT3D();
		Transformation3D position(_x, _y, 0.0, 0.0, 0.0, _yaw);
		Transformation3D delta(delta_x*cos(delta_th), delta_y*sin(delta_th), 0, 0, 0, delta_th);

		Transformation3D newposition = position * delta;

		_x = newposition.position.x;
		_y = newposition.position.y;

		double roll, pitch, yaw;
		newposition.orientation.getRPY(roll, pitch, yaw);
		_yaw = yaw;

		setAbsoluteT3D(newposition);

		if (b_use_collision)
		{
			World* world = getWorld();
			if (world) {
				if (world->checkCollisionWith(*this)) {
					setAbsoluteT3D(position); //no muevo el robot
					return false;
				}
			}
		}
		

		return true; //se pudo realizar el movimiento
	}

	//bool Ship::dynamicsSim(double delta_t)
	//{
	//
	//	_Wind_Force_Drag = sdl('a','f','d');// Valores demasiado altos
	//	_Water_Force_Drag = sdl('w', 'f', 'd');

	//	_Wind_Force_Side = sdl('a', 'f', 's');
	//	_Water_Force_Side = sdl('w', 'f', 's');

	//	_Wind_Moment = sdl('a', 'm', 'y');
	//	_Water_Moment = sdl('w', 'm', 'y');
	//	_Rotational_Moment = Mfront('a') + Mfront('w') + Mback('a') + Mback('w');

	//	double ax = (1 / _mass) * (_Wind_Force_Drag + _Water_Force_Drag + _thrust_x);
	//	double ay = (1 / _mass) * (_Wind_Force_Side + _Water_Force_Side + _thrust_y);
	//	double aw = (1 / _J) * (_Wind_Moment + _Water_Moment + /*_Rotational_Moment +*/ _thrust_y * _xp);
	//	// Hola mundo
	//	if (std::abs(ax) > _aMax)
	//		ax = _aMax * (ax / std::abs(ax));

	//	if (std::abs(ay) > _aMax)
	//		ay = _aMax * (ay / std::abs(ay));

	//	if (std::abs(aw) > _aMax)
	//		aw = _aMax * (aw / std::abs(aw));
	//	

	//	// MRUA
	//	double delta_x = _u * delta_t + 0.5 * ax * delta_t * delta_t;
	//	double delta_y = _v * delta_t + 0.5 * ay * delta_t * delta_t;

	//	// MCUA
	//	double delta_th = _w * delta_t + 0.5 * aw * delta_t * delta_t;;
	//	_move_success = false;

	//	_u += ax * delta_t;

	//	if (std::abs(_u) > _vMax)
	//		_u = _vMax * (_u/std::abs(_u));

	//	_v += ay * delta_t;

	//	if (std::abs(_v) > _vMax)
	//		_v = _vMax * (_v / std::abs(_v));

	//	_w += aw * delta_t;

	//	if (std::abs(_w) > _vMax)
	//		_w = _vMax * (_w / std::abs(_w));

	//	Transformation3D position = getAbsoluteT3D();
	//	Transformation3D delta(delta_x, delta_y, 0, 0, 0, delta_th);

	//	Transformation3D newposition = position * delta;

	//	setAbsoluteT3D(newposition);

	//	World* world = getWorld();
	//	if (world) {
	//		if (world->checkCollisionWith(*this)) {
	//			setAbsoluteT3D(position); //no muevo el robot
	//			return false;
	//		}
	//	}

	//	return true; //se pudo realizar el movimiento
	//}

	void Ship::drawGL()
	{

		ComposedEntity::drawGL();
		return;
	}

	bool Ship::setThrusts(double x, double y, double w)
	{
		_thrust_x = x;
		_thrust_y = y;
		_thrust_w = w;
		//if (_thrust_x > 10.0)_thrust_x = 10.0;
		//if (_thrust_x < 0.0)_thrust_x = 0.0;
		//if (_thrust_y > 10.0)_thrust_y = 10;
		//if (_thrust_y < 0.0)_thrust_y = 0.0;

		return true;
	}

	//double Ship::coeff(const double& x, const char& fluid, const char& type)
	//{
	//	// x is apparent flow angle in radians [-PI, PI]
	//	// fluid is either air(a) or water(w)
	//	// type is either drag(d), side(s), or yaw(y)

	//	if (fluid == 'a')
	//	{
	//		switch (type)
	//		{
	//		case 'd':
	//			//drag
	//			return (0.195738 + 0.518615 * std::abs(x) - 0.496029 * std::abs(x) * std::abs(x) +
	//				0.0941925 * std::abs(x) * std::abs(x) * std::abs(x) +
	//				1.86427 * sin(2 * PI * pow(std::abs(x) / PI, 1.05)) *
	//				exp(-2.17281 * (std::abs(x) - PI / 2) * (std::abs(x) - PI / 2)));
	//			break;

	//		case 's':
	//			// side
	//			if ((x < ((PI / 2)+0.1))&&(x > ((PI / 2) - 0.1)))
	//			{
	//				return _Crs_air;
	//				break;
	//			}
	//			else
	//			{
	//				double sgn = ((x > 0) ? 1 : -1);
	//				return (sgn * (12.3722 - 15.453 * std::abs(x) + 6.0261 * std::abs(x) -
	//					0.532325 * std::abs(x) * std::abs(x) * std::abs(x))
	//					* sin(std::abs(x)) *
	//					exp(-1.68668 * (std::abs(x) - PI / 2) * (std::abs(x) - PI / 2)));
	//				break;
	//			}


	//		case 'y':
	//		{
	//			double sgn = ((x > 0) ? 1 : -1);
	//			return (sgn * (0.710204 - 0.297196 * std::abs(x) +
	//				0.0857296 * std::abs(x) * std::abs(x)) *
	//				sin(2 * PI * pow(std::abs(x) / PI, 1.05)));
	//			break;
	//		}
	//			

	//		default:
	//			// wrong type input
	//			return -1;
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		if (fluid == 'w')
	//		{
	//			switch (type)
	//			{
	//				case 'd':
	//				{
	//					return (0.245219 - 0.93044 * std::abs(x) + 0.745752 * std::abs(x) *
	//						std::abs(x) - 0.15915 * std::abs(x) * std::abs(x) *
	//						std::abs(x) + 2.79188 * sin(2 * std::abs(x)) *
	//						exp(-1.05667 * (std::abs(x) - PI / 2) * (std::abs(x) - PI / 2)));
	//					break;
	//				}
	//				

	//				case 's':
	//				{
	//					if ((x < ((PI / 2) + 0.01)) && (x > ((PI / 2) - 0.01)))
	//					{
	//						return _Crs_water;
	//						break;
	//					}
	//					double sgn = ((x > 0) ? 1 : -1);
	//					return (sgn * (0.115554 + 3.09423 * std::abs(x) - 0.984923 * std::abs(x) *
	//						std::abs(x)) * sin(std::abs(x)));
	//					break;
	//				}
	//				
	//				case 'y':
	//				{
	//					double sgn = ((x > 0) ? 1 : -1);
	//					return (sgn * (0.322986 + 0.317964 * std::abs(x) - 0.1021844 * std::abs(x)
	//						* std::abs(x)) * sin(2 * std::abs(x)));
	//					break;
	//				}
	//				

	//				default:
	//				{
	//					// wrong type input
	//					return -1;
	//					break;
	//				}
	//				
	//			}
	//		}
	//		else
	//		{
	//			// wrong fluid input
	//			return -2;
	//		}
	//	}
	//}

	//double Ship::sdl(const char& fluid, const char& type, const char& type2)
	//{
	//	// Square Drag Law
	//	// x is apparent flow angle in radians [-PI, PI]
	//	// fluid is either air(a) or water(w)
	//	// type is eitcher force(f) or momentum(m)
	//	// type2 is either drag(d), side(s), or yaw(y)

	//	if (fluid == 'a')
	//	{
	//		double vr = sqrt(pow(_windSpeed * cos(_trueWindDirection - _yaw /*+ PI / 2*/) - _u, 2)     
	//		+ pow(_windSpeed * sin(_trueWindDirection - _yaw /*+ PI / 2*/) - _v, 2));

	//		if (type == 'f')
	//		{

	//			double apparent_dir = _trueWindDirection - _yaw;//+ PI / 2

	//			double aux_phi = atan((_windSpeed * sin(apparent_dir) -_v / (_windSpeed * cos(apparent_dir) - _u)));

	//			if ((_windSpeed * cos(apparent_dir) - _u) == 0)
	//				aux_phi = PI / 2.0;

	//			double sgn = 1;
	//			if(type2=='d')
	//			{
	//				if ((apparent_dir > (PI / 2.0)) && (apparent_dir < ((3.0/2.0) * PI)))
	//					sgn = -1.0;
	//			}
	//			else
	//			{
	//				if (type2 == 's')
	//				{
	//					if ((apparent_dir > PI) && (apparent_dir < (2.0 * PI)))
	//						sgn = -1.0;
	//				}
	//			}
	//			
	//			double aux_coeff = coeff(aux_phi, fluid, type2);
	//			return (sgn * 0.5 * aux_coeff * _Sair * _ro_air * vr * vr);

	//		}
	//		else
	//		{
	//			if (type == 'm')
	//			{
	//				double apparent_dir = _trueWindDirection - _yaw;//+ PI / 2

	//				double aux_phi = atan((_windSpeed * sin(apparent_dir) - _v / (_windSpeed * cos(apparent_dir) - _u)));

	//				if ((_windSpeed * cos(apparent_dir) - _u) == 0)
	//					aux_phi = PI / 2.0;

	//				double sgn = 1;
	//		
	//				if ((apparent_dir > PI) && (apparent_dir < (2.0 * PI)))
	//					sgn = -1.0;
	//	

	//				double aux_coeff = coeff(aux_phi, fluid, type2);

	//				return (sgn * 0.5 * aux_coeff * _Marm * _Sair * _ro_air * vr * vr);
	//			}
	//			else
	//			{
	//				// wrong type input
	//				return -1;
	//			}
	//		}

	//	}
	//	else
	//	{
	//		if (fluid == 'w')
	//		{
	//			double vr = sqrt(pow(_waterSpeed * cos(_trueWaterDirection - _yaw /*+ PI / 2*/) - _u, 2)
	//				+ pow(_waterSpeed * sin(_trueWaterDirection - _yaw /*+ PI / 2*/) - _v, 2));

	//			if (type == 'f')
	//			{

	//				// DEBUGGING
	//				double apparent_dir = _trueWaterDirection - _yaw;//+ PI / 2

	//				double aux_phi = atan((_waterSpeed * sin(apparent_dir) - _v) / (_waterSpeed * cos(apparent_dir) - _u));

	//				if ((_waterSpeed * cos(apparent_dir) - _u) == 0)
	//					aux_phi = PI / 2.0;

	//				double sgn = 1.0;
	//				if (type2 == 'd')
	//				{
	//					if ((apparent_dir > (PI / 2.0)) && (apparent_dir < (3.0 * PI / 2.0)))
	//						sgn = -1.0;
	//				}
	//				else
	//				{
	//					if (type2 == 's')
	//					{
	//						if ((apparent_dir > (PI / 2.0)) && (apparent_dir < (2.0 * PI)))
	//							sgn = -1.0;
	//					}
	//				}

	//				double aux_coeff = coeff(aux_phi, fluid, type2);

	//				return (sgn * 0.5 * aux_coeff * _Swater * _ro_water * vr * vr);
	//			}
	//			else
	//			{
	//				if (type == 'm')
	//				{
	//					double apparent_dir = _trueWaterDirection - _yaw ;//+ PI / 2

	//					double aux_phi = atan((_waterSpeed * sin(apparent_dir) - _v) / (_waterSpeed * cos(apparent_dir) - _u));

	//					if ((_waterSpeed * cos(apparent_dir) - _u) == 0)
	//						aux_phi = PI / 2.0;

	//					double sgn = 1;

	//					if ((apparent_dir > PI) && (apparent_dir < (2.0 * PI)))
	//						sgn = -1.0;

	//					double aux_coeff = coeff(aux_phi, fluid, type2);
	//					return (sgn * 0.5 * aux_coeff * _Marm * _Swater * _ro_water * vr * vr);
	//				}
	//				else
	//				{
	//					// wrong type input
	//					return -1;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			// wrong fluid input
	//			return -2;
	//		}
	//	}

	//}

	//double Ship::Mfront(const char& fluid)
	//{
	//	// fluid is either air(a) or water(w)

	//	double alpha_aux;
	//	if (fluid == 'a')
	//	{
	//		alpha_aux = _alpha_air;
	//	}
	//	else
	//	{
	//		if (fluid == 'w')
	//		{
	//			alpha_aux = _alpha_water;
	//		}
	//		else
	//		{
	//			// wrong fluid input
	//			return -2;
	//		}
	//	}
	//	double aux = ((_length * _length * alpha_aux / 192) * (3 * _length * _length * _w * _w + 16 * _length * _w * _v + 24 * _v * _v));
	//	return aux;
	//}

	//double Ship::Mback(const char& fluid)
	//{
	//	// fluid is either air(a) or water(w)

	//	double alpha_aux;
	//	if (fluid == 'a')
	//	{
	//		alpha_aux = _alpha_air;
	//	}
	//	else
	//	{
	//		if (fluid == 'w')
	//		{
	//			alpha_aux = _alpha_water;
	//		}
	//		else
	//		{
	//			// wrong fluid input
	//			return -2;
	//		}
	//	}
	//
	//	if(_v > (_w * _length / 2))
	//	{
	//		double aux = ((-1 * _length * _length * alpha_aux / 192) * (3 * _length * _length * _w * _w - 16 * _length * _w * _v + 24 * _v * _v));
	//		return aux;
	//	}
	//	else
	//	{
	//		double _w_aux = _w;
	//		if (_w_aux == 0)
	//			_w_aux = 0.001;
	//		double aux = (alpha_aux / (192 * _w_aux * _w_aux) * ((_length * _w_aux - 2 * _v) * (_length * _w_aux - 2 * _v) * (_length * _w_aux - 2 * _v) * (3 * _length * _w_aux + 2 * _v) - 16 * _v * _v * _v * _v));
	//		return aux;
	//	}
	//}

}
