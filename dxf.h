namespace dxf
{
	class Factory
	{
	public:
		virtual void circle(int handle, float center[3], float radius, float thickness=0, int color=0)=0;
		virtual void arc(int handle, float center[3], float radius, float start_angle, float end_angle, float thickness=0, int color=0)=0;
		virtual void line(int handle, float p1[3], float p2[3], float thickness=0, int color=0)=0;
		virtual void ellipse(int handle, float center[3], float major[3], float ratio, float start_angle, float end_angle, float thickness=0, int color=0)=0;
		virtual void point(int handle, float pt[3], float thickness=0, int color=0)=0;
		virtual void spline(int handle, float thickness=0, int color=0)=0;
	};
	void parse(std::istream &stream, Factory &factory);
}