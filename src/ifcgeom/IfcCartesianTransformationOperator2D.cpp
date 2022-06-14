/********************************************************************************
 *                                                                              *
 * This file is part of IfcOpenShell.                                           *
 *                                                                              *
 * IfcOpenShell is free software: you can redistribute it and/or modify         *
 * it under the terms of the Lesser GNU General Public License as published by  *
 * the Free Software Foundation, either version 3.0 of the License, or          *
 * (at your option) any later version.                                          *
 *                                                                              *
 * IfcOpenShell is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * Lesser GNU General Public License for more details.                          *
 *                                                                              *
 * You should have received a copy of the Lesser GNU General Public License     *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.         *
 *                                                                              *
 ********************************************************************************/

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Mat.hxx>
#include <gp_Mat2d.hxx>
#include <gp_GTrsf.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Circ.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Solid.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <TopLoc_Location.hxx>
#include "../ifcgeom/IfcGeom.h"

#define Kernel MAKE_TYPE_NAME(Kernel)

bool IfcGeom::Kernel::convert(const IfcSchema::IfcCartesianTransformationOperator2D* l, gp_Trsf2d& trsf) {
	IN_CACHE(IfcCartesianTransformationOperator2D,l,gp_Trsf2d,trsf)

	gp_Pnt origin;
	gp_Dir axis1 (1.,0.,0.);
	gp_Dir axis2 (0.,1.,0.);
	
	IfcGeom::Kernel::convert(l->LocalOrigin(),origin);
	if ( l->Axis1() ) IfcGeom::Kernel::convert(l->Axis1(),axis1);
	if ( l->Axis2() ) IfcGeom::Kernel::convert(l->Axis2(),axis2);
	
	const gp_Pnt2d origin2d(origin.X(), origin.Y());
	const gp_Dir2d axis12d(axis1.X(), axis1.Y());
	const gp_Dir2d axis22d(axis2.X(), axis2.Y());

	// A better match to represent the IfcCartesianTransformationOperator2D would
	// be the gp_Ax22d, but to my knowledge no easy way exists to convert it into
	// a gp_Trsf2d. Easiest would probably be to simply update the underlying
	// gp_Mat2d directly.
	
	const gp_Ax2d ax2d (origin2d, axis12d);
	trsf.SetTransformation(ax2d);
	
	if ( ax2d.Direction().Rotated(M_PI / 2.).Dot(axis22d) < 0. ) {
		gp_Trsf2d mirror; mirror.SetMirror(ax2d);
		trsf.Multiply(mirror);
	}

	trsf.Invert();
	if (l->Scale() && !ALMOST_THE_SAME(*l->Scale(), 1.)) {
		trsf.SetScaleFactor(*l->Scale());
	}

	if (is_identity(trsf, getValue(GV_PRECISION))) {
		trsf = gp_Trsf2d();
	}

	CACHE(IfcCartesianTransformationOperator2D,l,trsf)
	return true;
}
