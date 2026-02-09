#pragma once

class VertexBuffer
{
public:
	VertexBuffer(ComPtr<ID3D11Device> device);
	~VertexBuffer();

	ComPtr<ID3D11Buffer> GetComPtr() { return _vertexBuffer; }
	uint32 GetStride() { return _stride; }
	uint32 GetOffset() { return _offset; }
	uint32 GetCount() { return _count; }

	template<typename T>
	void Create(const vector<T>& vertices) {
		_stride = sizeof(T);
		_count = static_cast<uint32>(vertices.size());

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// Immutable: GPU에 의해서 읽을수만 있음 (CPU는 접근 불가)
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// -> Vertex Buffer를 만드는 데에 사용하겠다
		desc.ByteWidth = (uint32)(_stride * _count);

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices.data();	// 첫 번째 데이터의 메모리상 위치

		// 버퍼 생성
		HRESULT hr = _device->CreateBuffer(
			&desc,
			&data,
			_vertexBuffer.GetAddressOf()
		);

		CHECK(hr);
	}

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11Buffer> _vertexBuffer;

	uint32 _stride = 0;
	uint32 _offset = 0;
	uint32 _count = 0;

};

