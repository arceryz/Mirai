#include "Ray3Scene.h"
#include <raymath.h>
#include <unordered_map>
#include <string>

Ray3Scene::Ray3Scene()
{

}
void Ray3Scene::AddMirrorModel(Model model, bool flipNormals)
{
    for (int i = 0; i < model.meshCount; i++) {
        AddMirrorMesh(model.meshes[i], flipNormals);
    }
}
void Ray3Scene::AddMirrorMesh(Mesh mesh, bool flipNormals)
{
    int vertexCount = mesh.triangleCount*3;
    vector<Vector3> vertices;
    vector<Vector3> normals;
    vertices.resize(vertexCount);
    normals.resize(vertexCount);

    // Flatten everything to plain lists.
    for (int i = 0; i < vertexCount; i++) {
        int index = mesh.indices != NULL ? mesh.indices[i]: i;
        vertices[i] = ((Vector3*)mesh.vertices)[index];
        normals[i] = ((Vector3*)mesh.normals)[index];
    }

    // This map partitions all triangles by colinearity.
    // Using the inner product <n, x> = CONST and normal as string keys.
    unordered_map<string, vector<int>> triangleSets;

    for (int i = 0; i < vertexCount; i += 3) {
        Vector3 normal = normals[i];
        float dotp = Vector3DotProduct(normal, vertices[i]);
        string key = string(TextFormat("%1.3f:%1.3f:%1.3f:%.3f",
            normal.x, normal.y, normal.z, dotp));

        // Group triangles in triangle sets.
        vector<int> &tris = triangleSets[key];
        tris.push_back(i);
        tris.push_back(i+1);
        tris.push_back(i+2);
    }

    // For each set of colinear triangles we create a triangle net and
    // extract the polygons from it.
    for (auto kv: triangleSets) {
        // Form the triangle net.
        vector<Vector3> triangles;
        for (int i: kv.second) {
            triangles.push_back(vertices[i]);
        }
        TriangleNet net;
        net.AddTriangles(triangles);
        vector<Polygon> list = net.GetPolygons();
        for (Polygon &poly: list) {
            poly.normal = Vector3Scale(normals[kv.second[0]], flipNormals ? -1: 1);
        }

        printf("Subnet consists of %d polygons\n", list.size());
        mirrors.insert(mirrors.end(), list.begin(), list.end());
    }
    printf("Extracted %d mirrors\n", mirrors.size());
}
void Ray3Scene::DebugDrawMesh(Mesh mesh)
{
    Vector3 *vertices = (Vector3*)mesh.vertices;
    Vector3 *normals = (Vector3*)mesh.normals;

    for (int i = 0; i < mesh.triangleCount*3; i++) {
        Vector3 v = vertices[mesh.indices[i]];
        Vector3 n = Vector3Add(v, Vector3Scale(normals[mesh.indices[i]], 0.3));

        DrawSphere(v, 0.05, BLUE);
        DrawLine3D(v, n, GREEN);
    }
}
void Ray3Scene::DrawMirrors(vector<Color> colors, float faceScale)
{
    for (int i = 0; i < mirrors.size(); i++) {
        Color color = colors[i%colors.size()];
        mirrors[i].Draw(color, color, faceScale);
    }
}