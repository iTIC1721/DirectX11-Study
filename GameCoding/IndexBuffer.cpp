#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(ComPtr<ID3D11Device> device) 
	: _device(device)
{

}

IndexBuffer::~IndexBuffer() 
{

}

void IndexBuffer::Create(const vector<uint32>& indices) 
{
	_stride = sizeof(uint32);
	_count = static_cast<uint32>(indices.size());

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;	// Immutable: GPU에 의해서 읽을수만 있음 (CPU는 접근 불가)
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// -> Vertex Buffer를 만드는 데에 사용하겠다
	desc.ByteWidth = (uint32)(_stride * _count);

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = indices.data();	// 첫 번째 데이터의 메모리상 위치

	// 버퍼 생성
	HRESULT hr = _device->CreateBuffer(
		&desc,
		&data,
		_indexBuffer.GetAddressOf()
	);

	CHECK(hr);
}