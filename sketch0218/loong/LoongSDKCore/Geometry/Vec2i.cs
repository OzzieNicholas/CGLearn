using System;
using System.Collections.Generic;
using System.Drawing;

namespace LoongCAD.Geometry
{
	
	/**
	2-int-components vector
	*/
	[Serializable]
	public struct Vec2i
	{
		public int X;
		public int Y;

		public static readonly Vec2i ZERO = new Vec2i(0, 0);
		public static readonly Vec2i MAX = new Vec2i(Int32.MaxValue, Int32.MaxValue);
		public static readonly Vec2i MIN = new Vec2i(Int32.MinValue, Int32.MinValue);
		public static readonly Vec2i AXIS_X = new Vec2i(1, 0);
		public static readonly Vec2i AXIS_Y = new Vec2i(0, 1);

		public Vec2i(int _x, int _y) {
			X = _x;
			Y = _y;
		}

		public Vec2i(Vec2i v) {
			X = v.X;
			Y = v.Y;
		}

		public override bool Equals(object obj) {
			if (!(obj is Vec2i))
				return false;

			Vec2i vec = (Vec2i)obj;
			return X == vec.X && Y == vec.Y;
		}

		public override int GetHashCode() {
			return Tuple.Create(X, Y).GetHashCode();
		}

		public override string ToString() {
			return "" + X + ',' + Y;
		}

		public Vec2 ToVec2() {
			return new Vec2((double)X, (double)Y);
		}

		public Point ToPoint() {
			return new Point((int)X, (int)Y);
		}

		public PointF ToPointf() {
			return new PointF((float)X, (float)Y);
		}

		public Size ToSize() {
			return new Size((int)X, (int)Y);
		}

		public void Set(int _x, int _y) {
			X = _x;
			Y = _y;
		}

		public int this[int idx] {
			get => (idx == 0) ? X : (idx == 1) ? Y : 0;
			set {
				switch (idx) {
					case 0:
					X = value;
					return;
					case 1:
					Y = value;
					return;
				}
			}
		}

		public static bool operator ==(Vec2i a, Vec2i b) {
			return a.X == b.X && a.Y == b.Y;
		}

		public static bool operator !=(Vec2i a, Vec2i b) {
			return a.X != b.X || a.Y != b.Y;
		}

		public static Vec2i operator +(Vec2i vec0, Vec2i vec1) {
			return new Vec2i(vec0.X + vec1.X, vec0.Y + vec1.Y);
		}

		public static Vec2i operator +(Vec2i vec, int v) {
			return new Vec2i(vec.X + v, vec.Y + v);
		}

		public static Vec2i operator +(int v, Vec2i vec) {
			return new Vec2i(vec.X + v, vec.Y + v);
		}

		public static Vec2i operator -(Vec2i vec) {
			return new Vec2i(-vec.X, -vec.Y);
		}

		public static Vec2i operator -(Vec2i vec0, Vec2i vec1) {
			return new Vec2i(vec0.X - vec1.X, vec0.Y - vec1.Y);
		}

		public static Vec2i operator -(Vec2i vec, int v) {
			return new Vec2i(vec.X - v, vec.Y - v);
		}

		public static Vec2i operator -(int v, Vec2i vec) {
			return new Vec2i(v - vec.X, v - vec.Y);
		}

		public static Vec2i operator *(Vec2i vec0, Vec2i vec1) {
			return new Vec2i(vec0.X * vec1.X, vec0.Y * vec1.Y);
		}

		public static Vec2i operator *(Vec2i vec, int v) {
			return new Vec2i(vec.X * v, vec.Y * v);
		}

		public static Vec2i operator *(int v, Vec2i vec) {
			return new Vec2i(vec.X * v, vec.Y * v);
		}

		public static Vec2i operator /(Vec2i vec0, Vec2i vec1) {
			return new Vec2i(vec0.X / vec1.X, vec0.Y / vec1.Y);
		}

		public static Vec2i operator /(Vec2i vec, int v) {
			return new Vec2i(vec.X / v, vec.Y / v);
		}

		public static Vec2i operator /(int v, Vec2i vec) {
			return new Vec2i(v / vec.X, v / vec.Y);
		}
	}
}
