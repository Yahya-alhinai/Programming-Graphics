/** CSci-4611 Assignment 3:  Earthquake
 */

#include "earth.h"
#include "config.h"

#include <vector>

// for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>


Earth::Earth() {
}

Earth::~Earth() {
}

float normalize_x(float X) {
	return (X + M_PI) / (2 * M_PI);
}

float normalize_y(float Y) {
	return -(Y - 0.5*M_PI) / (M_PI);
}

void Earth::Init(const std::vector<std::string> &search_path) {
    // init shader program
    shader_.Init();
    
    // init texture: you can change to a lower-res texture here if needed
    earth_tex_.InitFromFile(Platform::FindFile("earth-2k.png", search_path));

    // init geometry
	const int nslices = 30;
    const int nstacks = 30;

    // TODO: This is where you need to set the vertices and indiceds for earth_mesh_.
	std::vector<Point2> tex_coords;

    // As a demo, we'll add a square with 2 triangles.
    std::vector<unsigned int> indices;
	
	float x_init = -M_PI;
	float y_init = M_PI/2;
	float x_step = (2 * M_PI / nslices);
	float y_step = (M_PI / nstacks);


	for (float i = 0; i <= nstacks; i++) {
		vertices_plane.push_back(Point3(x_init, y_init - i * y_step, 0));
		vertices_sphere.push_back(LatLongToSphere(x_init, y_init - i * y_step));
		tex_coords.push_back(Point2(normalize_x(vertices_plane.at(vertices_plane.size() - 1)[0]), normalize_y(vertices_plane.at(vertices_plane.size() - 1)[1])));

		for (float j = 1; j <= nslices; j ++) {
			vertices_plane.push_back(Point3(x_init + j * x_step, y_init - i * y_step, 0));
			vertices_sphere.push_back(LatLongToSphere(x_init + j * x_step, y_init - i * y_step));
			tex_coords.push_back(Point2(normalize_x(vertices_plane.at(vertices_plane.size() - 1)[0]), normalize_y(vertices_plane.at(vertices_plane.size() - 2)[1])));
		}
		if (i == 0) continue;


		for (float j = 0; j < nslices; j++) {
			unsigned idx[4] = {
				vertices_plane.size() - 2 * nslices - 2 + j,
				vertices_plane.size() - nslices - 1 + j,
				vertices_plane.size() - nslices + j,
				vertices_plane.size() - 2 * nslices - 1 + j
			};

			Point3 Val[4] = {
				vertices_plane[idx[0]],
				vertices_plane[idx[1]],
				vertices_plane[idx[2]],
				vertices_plane[idx[3]]
			};//vertices_sphere

			Point3 Val_sphere[4] = {
				vertices_sphere[idx[0]],
				vertices_sphere[idx[1]],
				vertices_sphere[idx[2]],
				vertices_sphere[idx[3]]
			};


			// indices into the arrays above for the first triangle
			indices.push_back(idx[0]);
			indices.push_back(idx[1]);
			indices.push_back(idx[2]);
			normals_plane.push_back(((Val[0] - Val[1]).Cross(Val[1] - Val[3])).ToUnit());
			normals_sphere.push_back(((vertices_sphere[0] - vertices_sphere[1]).Cross(vertices_sphere[1] - vertices_sphere[3])).ToUnit());


			// indices for the second triangle, note some are reused
			indices.push_back(idx[0]);
			indices.push_back(idx[2]);
			indices.push_back(idx[3]);
			normals_plane.push_back(((Val[0] - Val[2]).Cross(Val[2] - Val[3])).ToUnit());
			normals_sphere.push_back(((vertices_sphere[0] - vertices_sphere[2]).Cross(vertices_sphere[2] - vertices_sphere[3])).ToUnit());

		}
	}

	earth_mesh_.SetVertices(vertices_plane);
	earth_mesh_.SetIndices(indices);
	earth_mesh_.SetNormals(normals_plane);
	earth_mesh_.SetTexCoords(0, tex_coords);

    earth_mesh_.UpdateGPUMemory();
}


void Earth::Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // Define a really bright white light.  Lighting is a property of the "shader"
    DefaultShader::LightProperties light;
    light.position = Point3(10,10,10);
    light.ambient_intensity = Color(1,1,1);
    light.diffuse_intensity = Color(1,1,1);
    light.specular_intensity = Color(1,1,1);
    shader_.SetLight(0, light);

    // Adust the material properties, material is a property of the thing
    // (e.g., a mesh) that we draw with the shader.  The reflectance properties
    // affect the lighting.  The surface texture is the key for getting the
    // image of the earth to show up.
    DefaultShader::MaterialProperties mat;
    mat.ambient_reflectance = Color(0.5, 0.5, 0.5);
    mat.diffuse_reflectance = Color(0.75, 0.75, 0.75);
    mat.specular_reflectance = Color(0.75, 0.75, 0.75);
    mat.surface_texture = earth_tex_;

    // Draw the earth mesh using these settings
    if (earth_mesh_.num_triangles() > 0) {
        shader_.Draw(model_matrix, view_matrix, proj_matrix, &earth_mesh_, mat);
    }



}


Point3 Earth::LatLongToSphere(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all yours
    // lat,long --> sphere calculations in one place.
	return Point3(cos(longitude) * sin(latitude), sin(longitude), cos(longitude) * cos(latitude));
}

Point3 Earth::LatLongToPlane(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all your
    // lat,long --> plane calculations in one place.
    return Point3(GfxMath::ToRadians(longitude), GfxMath::ToRadians(latitude), 0);
}



void Earth::DrawDebugInfo(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // This draws a cylinder for each line segment on each edge of each triangle in your mesh.
    // So it will be very slow if you have a large mesh, but it's quite useful when you are
    // debugging your mesh code, especially if you start with a small mesh.
    for (int t=0; t<earth_mesh_.num_triangles(); t++) {
        std::vector<unsigned int> indices = earth_mesh_.triangle_vertices(t);
        std::vector<Point3> loop;
        loop.push_back(earth_mesh_.vertex(indices[0]));
        loop.push_back(earth_mesh_.vertex(indices[1]));
        loop.push_back(earth_mesh_.vertex(indices[2]));
		quick_shapes_.DrawLines(model_matrix, view_matrix, proj_matrix,
			Color(1, 1, 0), loop, QuickShapes::LinesType::LINE_LOOP, 0.005);
    }

}

