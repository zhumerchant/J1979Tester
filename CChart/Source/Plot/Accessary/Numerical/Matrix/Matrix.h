#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::istream;
using std::ostream;

// �������;�����
template <typename Object>
class MATRIX
{
public:
	explicit MATRIX() : array( 0 ) {}

	MATRIX( int rows, int cols):array( rows )
	{
		for( int i = 0; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}

	MATRIX( const MATRIX<Object>& m ){ *this = m;}

	void resize( int rows, int cols );           // �ı䵱ǰ�����С
	bool push_back( const vector<Object>& v );   // �ھ���ĩβ���һ������
	void swap_row( int row1, int row2 );         // �������е�����

	int  rows() const{ return array.size(); }
	int  cols() const { return rows() ? (array[0].size()) : 0; }
	bool empty() const { return rows() == 0; }        // �Ƿ�Ϊ��
	bool square() const { return (!(empty()) && rows() == cols()); }  // �Ƿ�Ϊ����
	
	
	const vector<Object>& operator[](int row) const { return array[row]; } //[]���������� 
	vector<Object>& operator[](int row){ return array[row]; }
	Object &ele(int col, int row){ return array[row][col]; }
	
protected:
	vector< vector<Object> > array;
};

// �ı䵱ǰ�����С
template <typename Object>
void MATRIX<Object>::resize( int rows, int cols )
{
	int rs = this->rows();
	int cs = this->cols();

	if ( rows == rs && cols == cs )
	{
		return;
	}
	else if ( rows == rs && cols != cs )
	{
		for ( int i = 0; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}
	else if ( rows != rs && cols == cs )
	{
		array.resize( rows );
		for ( int i = rs; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}
	else
	{
		array.resize( rows );
		for ( int i = 0; i < rows; ++i )
		{
			array[i].resize( cols );
		}
	}
}

// �ھ���ĩβ���һ��
template <typename Object>
bool MATRIX<Object>::push_back( const vector<Object>& v )
{
	if ( rows() == 0 || cols() == (int)v.size() )
	{
		array.push_back( v );
	}
	else
	{
		return false;
	}

	return true;
}

// ��������
template <typename Object>
void MATRIX<Object>::swap_row( int row1, int row2 )
{
	if ( row1 != row2 && row1 >=0 &&
		row1 < rows() && row2 >= 0 && row2 < rows() )
	{
		vector<Object>& v1 = array[row1];
		vector<Object>& v2 = array[row2];
		vector<Object> tmp = v1;
		v1 = v2;
		v2 = tmp;
	}
}

// ����ת��
template <typename Object>
const MATRIX<Object> trans( const MATRIX<Object>& m )
{
	MATRIX<Object> ret;
	if ( m.empty() ) return ret;

	int row = m.cols();
	int col = m.rows();
	ret.resize( row, col );

	for ( int i = 0; i < row; ++i )
	{
		for ( int j = 0; j < col; ++j )
		{
			ret[i][j] = m[j][i];
		}
	}

	return ret;
}

//////////////////////////////////////////////////////////
// double���;����࣬���ڿ�ѧ����
// �̳���MATRIX��
// ʵ�ֳ��ò��������أ���ʵ�ּ�����������ʽ�����Լ�LU�ֽ�
class Matrix:public MATRIX<double>
{
public:
	Matrix():MATRIX<double>(){}
	Matrix( int c, int r ):MATRIX<double>(c,r){}
	Matrix( const Matrix& m){ *this  = m; }

	const Matrix& operator+=( const Matrix& m );
	const Matrix& operator-=( const Matrix& m );
	const Matrix& operator*=( const Matrix& m );
	const Matrix& operator/=( const Matrix& m );
};

bool  operator==( const Matrix& lhs, const Matrix& rhs );        // ���ز�����==
bool  operator!=( const Matrix& lhs, const Matrix& rhs );        // ���ز�����!=
const Matrix operator+( const Matrix& lhs, const Matrix& rhs );  // ���ز�����+
const Matrix operator-( const Matrix& lhs, const Matrix& rhs );  // ���ز�����-
const Matrix operator*( const Matrix& lhs, const Matrix& rhs );  // ���ز�����*
const Matrix operator/( const Matrix& lhs, const Matrix& rhs );  // ���ز�����/
const double det( const Matrix& m );                             // ��������ʽ
const double det( const Matrix& m, int start, int end );         // �����Ӿ�������ʽ
const Matrix abs( const Matrix& m );                             // ��������Ԫ�صľ���ֵ
const double max( const Matrix& m );                             // ����Ԫ�ص����ֵ
const double max( const Matrix& m, int& row, int& col);          // ����Ԫ���е����ֵ�����±�
const double min( const Matrix& m );                             // ����Ԫ�ص���Сֵ
const double min( const Matrix& m, int& row, int& col);          // ����Ԫ�ص���Сֵ�����±�
const Matrix trans( const Matrix& m );                           // ����ת�þ���
const Matrix submatrix(const Matrix& m,int rb,int re,int cb,int ce);  // �����Ӿ���
const Matrix inverse( const Matrix& m );                         // ���������
const Matrix LU( const Matrix& m );                              // ���㷽���LU�ֽ�
const Matrix readMatrix( istream& in = std::cin );               // ��ָ���������������
const Matrix readMatrix( string file );                          // ���ı��ļ��������
const Matrix loadMatrix( string file );                          // �Ӷ������ļ���ȡ����
void  printMatrix( const Matrix& m, ostream& out = std::cout );  // ��ָ���������ӡ����
void  printMatrix( const Matrix& m, string file);                // ������������ı��ļ�
void  saveMatrix( const Matrix& m, string file);                 // �����󱣴�Ϊ�������ļ�

#endif
