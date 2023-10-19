//
// Created by summernight on 07.10.23.
//

#include "cuban.h"
#include "bgfx/bgfx.h"
#include "bx/math.h"
#ifdef __APPLE__
#include <vs_shader.sc.mtl.bin.h>
#include <fs_shader.sc.mtl.bin.h>
#define vertexPivko vs_shader_mtl
#define fragmentPivko fs_shader_mtl
#endif
#ifdef __linux__
#include <vs_shader.sc.spv.bin.h>
#include <fs_shader.sc.spv.bin.h>
#define vertexPivko vs_shader_spv
#define fragmentPivko fs_shader_spv
#endif
#include <vector>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

static const aiScene* DoTheImportThing( const std::string& pFile) {
    // Create an instance of the Importer class
    auto* importer = new Assimp::Importer();

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer->ReadFile( pFile,
                                              aiProcess_CalcTangentSpace       |
                                              aiProcess_Triangulate            |
                                              aiProcess_JoinIdenticalVertices  |
                                              aiProcess_SortByPType);

    return scene;
}

    struct PosColorVertex
    {
        float m_x;
        float m_y;
        float m_z;
        uint32_t m_abgr;
        static void init()
        {
            ms_layout
                    .begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                    .end();
        };
        static bgfx::VertexLayout ms_layout;
    };
    bgfx::VertexLayout PosColorVertex::ms_layout;
    static PosColorVertex s_cubeVertices[] =
            {
                    {  0.5f,  0.5f, 0.0f, 0xff0000ff },
                    {  0.5f, -0.5f, 0.0f, 0x0000ffff },
                    { -0.5f, -0.5f, 0.0f, 0x00ff0000 },
                    { -0.5f,  0.5f, 0.0f, 0x00000000 }
            };
    static const uint16_t s_cubeTriList[] =
            {
                    0,1,3,
                    1,2,3
            };

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::ProgramHandle m_program;

void Cuban::init() {
    isInited = true;
    PosColorVertex::init();

    auto kekScene = DoTheImportThing("/Users/vlad-025/Desktop/projs/engine/assets/colladas/import_test_kek.dae");

    std::vector<PosColorVertex> vertices;

    auto mesh = kekScene->mMeshes[0];
    vertices.reserve(mesh->mNumVertices * 3);
    for(int j = 0; j < mesh->mNumVertices; j++) {
        auto vertice = mesh->mVertices[j];
        vertices.push_back( { vertice.x / 2, vertice.y / 2, 0, 0xff0000ff } );
    }

    std::vector<unsigned int> indices;
    // Extracting all faces (indices)
    for (int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

//    // Filling array of vertices
//    for (int i = 0; i < kekScene->mNumMeshes; i++) {
//        auto mesh = kekScene->mMeshes[i];
//        for(int j = 0; j < mesh->mNumVertices; j++) {
//            vertices.push_back(mesh->mVertices[j]);
//        }
//    }

    auto vertData = vertices.data();
    m_vbh = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
            PosColorVertex::ms_layout
    );

    auto indexData = indices.data();
    m_ibh = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );

    const bgfx::Memory* memVertex = bgfx::copy(vertexPivko, sizeof(vertexPivko));
    bgfx::ShaderHandle handleVertex = bgfx::createShader(memVertex);

    const bgfx::Memory* memFragment = bgfx::copy(fragmentPivko, sizeof(fragmentPivko));
    bgfx::ShaderHandle handleFragment = bgfx::createShader(memFragment);

    m_program = bgfx::createProgram(handleVertex, handleFragment,  true);
}

void Cuban::update() {
//    bgfx::touch(0);
//    float mt x[16];
//    bx::mtxRotateY(mtx, 0.0f);
//    // position x,y,z
//    mtx[12] = 0.0f;
//    mtx[13] = 0.0f;
//    mtx[14] = 0.0f;
//    // Set model matrix for rendering.
//    bgfx::setTransform(mtx);
    // Set vertex and index buffer.
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    // Submit primitive for rendering to view 0.
    bgfx::submit(0, m_program);
}