package com.dangeroushive.meshmedic

import com.google.android.filament.Box
import com.google.android.filament.Engine
import com.google.android.filament.EntityManager
import com.google.android.filament.IndexBuffer
import com.google.android.filament.Material
import com.google.android.filament.RenderableManager
import com.google.android.filament.VertexBuffer
import com.google.android.filament.filamat.MaterialBuilder
import java.nio.ByteBuffer
import java.nio.ByteOrder

class FilamentCube(private val engine: Engine) {
    private val entity = EntityManager.get().create()

    private lateinit var material: Material
    private lateinit var vertexBuffer: VertexBuffer
    private lateinit var indexBuffer: IndexBuffer

    fun build() {
        MaterialBuilder.init()

        val packageData = MaterialBuilder()
            .name("MeshMedicCube")
            .platform(MaterialBuilder.Platform.MOBILE)
            .targetApi(MaterialBuilder.TargetApi.ALL)
            .shading(MaterialBuilder.Shading.UNLIT)
            .uniformParameter(MaterialBuilder.UniformType.FLOAT4, "baseColor")
            .material(
                """
                void material(inout MaterialInputs material) {
                    prepareMaterial(material);
                    material.baseColor = materialParams.baseColor;
                }
                """.trimIndent()
            )
            .build(engine)

        check(packageData.isValid()) { "Failed to compile Mesh Medic cube material" }

        material = Material.Builder()
            .payload(packageData.getBuffer(), packageData.getBuffer().remaining())
            .build(engine)

        material.defaultInstance.setParameter("baseColor", 0.42f, 0.58f, 0.82f, 1.0f)

        val vertices = floatArrayOf(
            -1f, -1f,  1f,
             1f, -1f,  1f,
             1f,  1f,  1f,
            -1f,  1f,  1f,
            -1f, -1f, -1f,
             1f, -1f, -1f,
             1f,  1f, -1f,
            -1f,  1f, -1f
        )

        val indices = shortArrayOf(
            0, 1, 2, 0, 2, 3,
            5, 4, 7, 5, 7, 6,
            4, 0, 3, 4, 3, 7,
            1, 5, 6, 1, 6, 2,
            3, 2, 6, 3, 6, 7,
            4, 5, 1, 4, 1, 0
        )

        val vertexData = ByteBuffer
            .allocate(vertices.size * Float.SIZE_BYTES)
            .order(ByteOrder.nativeOrder())

        vertices.forEach(vertexData::putFloat)
        vertexData.flip()

        vertexBuffer = VertexBuffer.Builder()
            .bufferCount(1)
            .vertexCount(vertices.size / 3)
            .attribute(
                VertexBuffer.VertexAttribute.POSITION,
                0,
                VertexBuffer.AttributeType.FLOAT3,
                0,
                3 * Float.SIZE_BYTES
            )
            .build(engine)

        vertexBuffer.setBufferAt(engine, 0, vertexData)

        val indexData = ByteBuffer
            .allocate(indices.size * Short.SIZE_BYTES)
            .order(ByteOrder.nativeOrder())

        indices.forEach(indexData::putShort)
        indexData.flip()

        indexBuffer = IndexBuffer.Builder()
            .indexCount(indices.size)
            .bufferType(IndexBuffer.Builder.IndexType.USHORT)
            .build(engine)

        indexBuffer.setBuffer(engine, indexData)

        RenderableManager.Builder(1)
            .boundingBox(Box(-1f, -1f, -1f, 1f, 1f, 1f))
            .geometry(
                0,
                RenderableManager.PrimitiveType.TRIANGLES,
                vertexBuffer,
                indexBuffer,
                0,
                indices.size
            )
            .material(0, material.defaultInstance)
            .culling(false)
            .build(engine, entity)
    }

    fun addTo(scene: com.google.android.filament.Scene) {
        scene.addEntity(entity)
    }

    fun destroy(scene: com.google.android.filament.Scene) {
        scene.remove(entity)
        engine.destroyEntity(entity)
        engine.destroyVertexBuffer(vertexBuffer)
        engine.destroyIndexBuffer(indexBuffer)
        engine.destroyMaterial(material)
        MaterialBuilder.shutdown()
    }
}
