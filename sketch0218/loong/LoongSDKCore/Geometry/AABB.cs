using System;
using System.Collections.Generic;

namespace LoongCAD.Geometry
{
	[Serializable]
	public struct AABB
	{
		private Vec3 m_min;
		private Vec3 m_max;

		public AABB(Vec3 vMin, Vec3 vMax) {
			m_min = vMin;
			m_max = vMax;
		}

		public AABB(AABB src) {
			m_min = src.m_min;
			m_max = src.m_max;
		}

		public override bool Equals(object obj) {
			if (!(obj is AABB))
				return false;

			AABB vec = (AABB)obj;
			return m_min.Equals(vec.m_min) && m_max.Equals(vec.m_max);
		}

		public override int GetHashCode() {
			return Tuple.Create(m_min, m_max).GetHashCode();
		}

		public override string ToString() {
			return m_min.ToString() + ">>" + m_max.ToString();
		}

		public Vec3 Min {
			get => m_min;
			set => m_min = value;
		}

		public Vec3 Max {
			get => m_max;
			set => m_max = value;
		}

		public bool IsValid() {
			return m_min.X <= m_max.X && m_min.Y <= m_max.Y && m_min.Z <= m_max.Z;
		}

		void Reset() {
			m_min = Vec3.MAX;
			m_max = Vec3.MIN;
		}

		public void Add(Vec3 p) {
			m_min = m_min.Min(p);
			m_max = m_max.Max(p);
		}

		public bool Contains(Vec3 p) {
			return (p.X >= m_min.X) && (p.X <= m_max.X) &&
				(p.Y >= m_min.Y) && (p.Y <= m_max.Y) &&
				(p.Z >= m_min.Z) && (p.Z <= m_max.Z);
		}

		public void Inflate(double x, double y, double z) {
			m_min.X -= x;
			m_min.Y -= y;
			m_min.Z -= z;
			m_max.X += x;
			m_max.Y += y;
			m_max.Z += z;
		}

		public void Transform(Mat43 m) {
			Vec3[] v = new Vec3[] {
				m * new Vec3(m_min.X, m_min.Y, m_min.Z),
				m * new Vec3(m_max.X, m_min.Y, m_min.Z),
				m * new Vec3(m_min.X, m_max.Y, m_min.Z),
				m * new Vec3(m_min.X, m_min.Y, m_max.Z),
				m * new Vec3(m_max.X, m_max.Y, m_min.Z),
				m * new Vec3(m_min.X, m_max.Y, m_max.Z),
				m * new Vec3(m_max.X, m_min.Y, m_max.Z),
				m * new Vec3(m_max.X, m_max.Y, m_max.Z)
			};

			m_min = Vec3.MAX;
			m_max = Vec3.MIN;

			for (int i = 0; i < 8; ++i) {
				m_min.X = m_min.X < v[i].X ? m_min.X : v[i].X;
				m_min.Y = m_min.Y < v[i].Y ? m_min.Y : v[i].Y;
				m_min.Z = m_min.Z < v[i].Z ? m_min.Z : v[i].Z;

				m_max.X = m_max.X > v[i].X ? m_max.X : v[i].X;
				m_max.Y = m_max.Y > v[i].Y ? m_max.Y : v[i].Y;
				m_max.Z = m_max.Z > v[i].Z ? m_max.Z : v[i].Z;
			}
		}

		public Vec3[] Points => new Vec3[] {m_min,
											new Vec3(m_max.X, m_min.Y, m_min.Z),
											new Vec3(m_min.X, m_max.Y, m_min.Z),
											new Vec3(m_min.X, m_min.Y, m_max.Z),
											new Vec3(m_max.X, m_max.Y, m_min.Z),
											new Vec3(m_min.X, m_max.Y, m_max.Z),
											new Vec3(m_max.X, m_min.Y, m_max.Z),
											m_max };

		public bool IntersectRay(Vec3 rayPos, Vec3 rayDir) {
			double lowt = 0.0;
			double t = 0.0;
			bool hit = false;
			Vec3 hitpoint;
			Vec3 rayorig = rayPos;
			Vec3 raydir = rayDir;

			if (rayorig.X > m_min.X &&
				rayorig.Y > m_min.Y &&
				rayorig.Z > m_min.Z &&
				rayorig.X < m_max.X &&
				rayorig.Y < m_max.Y &&
				rayorig.Z < m_max.Z) {
				return true;
			}

			if (rayorig.X < m_min.X && raydir.X > 0) {
				t = (m_min.X - rayorig.X) / raydir.X;

				if (t > 0) {
					hitpoint = rayorig + raydir * t;
					if (hitpoint.Y >= m_min.Y && hitpoint.Y <= m_max.Y && hitpoint.Z >= m_min.Z && hitpoint.Z <= m_max.Z && (!hit || t < lowt)) {
						return true;
					}
				}
			}

			if (rayorig.X > m_max.X && raydir.X < 0) {
				t = (m_max.X - rayorig.X) / raydir.X;
				if (t > 0) {
					hitpoint = rayorig + raydir * t;
					if (hitpoint.Y > m_min.Y && hitpoint.Y <= m_max.Y &&
						hitpoint.Z >= m_min.Z && hitpoint.Z <= m_max.Z &&
						(!hit || t < lowt)) {
						return true;
					}
				}
			}

			if (rayorig.Y < m_min.Y && raydir.Y > 0) {
				t = (m_min.Y - rayorig.Y) / raydir.Y;
				if (t > 0) {
					hitpoint = rayorig + raydir * t;
					if (hitpoint.X >= m_min.X && hitpoint.X <= m_max.X &&
						hitpoint.Z >= m_min.Z && hitpoint.Z <= m_max.Z &&
						(!hit || t < lowt)) {
						return true;
					}
				}
			}

			if (rayorig.Y > m_max.Y && raydir.Y < 0) {
				t = (m_max.Y - rayorig.Y) / raydir.Y;
				if (t > 0) {
					hitpoint = rayorig + raydir * t;
					if (hitpoint.X >= m_min.X && hitpoint.X <= m_max.X &&
						hitpoint.Z >= m_min.Z && hitpoint.Z <= m_max.Z &&
						(!hit || t < lowt)) {
						return true;
					}
				}
			}

			if (rayorig.Z < m_min.Z && raydir.Z > 0) {
				t = (m_min.Z - rayorig.Z) / raydir.Z;
				if (t > 0) {
					hitpoint = rayorig + raydir * t;
					if (hitpoint.X >= m_min.X && hitpoint.X <= m_max.X &&
						hitpoint.Y >= m_min.Y && hitpoint.Y <= m_max.Y &&
						(!hit || t < lowt)) {
						return true;
					}
				}
			}

			if (rayorig.Z > m_max.Z && raydir.Z < 0) {
				t = (m_max.Z - rayorig.Z) / raydir.Z;
				if (t > 0) {
					hitpoint = rayorig + raydir * t;
					if (hitpoint.X >= m_min.X && hitpoint.X <= m_max.X &&
						hitpoint.Y >= m_min.Y && hitpoint.Y <= m_max.Y &&
						(!hit || t < lowt)) {
						return true;
					}
				}
			}

			return false;
		}
	}
}
