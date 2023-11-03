#############################################################################
## C/C++テーブル定義抽出スクリプト
##
## Version
## 1.0  2023.11.06　2D配列まで対応。3D以上、構造体には未対応。
## Author : K.Furuichi
#############################################################################
import re
import os
import sys
import numpy as np

'''入力文字列取得
'''
def GetInputString():
	# テスト用の文字列ret
#	path = os.getcwd() + "\\08_regrep_c_struct\\sample1.c"
#	path = os.getcwd() + "\\sample1.c"
#	fp = open(path)
	if len(sys.argv) < 2:
		print("Usage:")
		print("  $> python", sys.argv[0], "<input (*.[ch])>")
		return ''
	else:
		fp = open(sys.argv[1], "r")
	tmp_string = fp.read()
	return tmp_string

'''構造体抽出
'''
def ExtractStruct(input_string):
	l = input_string.split(';')
	return l

'''構造体要素抽出
@return 
	m.group(1) 構造体変数名
	m.group(2) 要素数
	m.group(3) 要素定義
'''
def ExtractStructCont(input_string):
	pattern = r'\b([a-zA-Z_][a-zA-Z0-9_]*)\[(.+)\]\s*=\s*\{(.*)\}'
	m = re.search(pattern, input_string)
	return m

'''配列要素分解
'''
def DivideElemenet(input_string):
	# 先頭末尾の空白削除し、要素ごと分解
	l = [i.strip() for i in input_string.split(',')]

	# DEBUG用：numpy arrny への変換に失敗した場合の確認用
	print("len=", len(l), " string[", input_string, "]" )
	
	return l

''''配列ブロック分解
xxx yyy[2][2][3] =
{
	{
		{1,2,3}, 
		{1,2,3}
	},
	{
		{4,5,6},
		{4,5,6}
	}
}
'''
def DivideElementBlock(input_string):
	len = 0
	# 空白削除
	tmp_string = re.sub(r'\s','', input_string)

	#DEBGU用
	#print("tmp_string: ",tmp_string)

	if tmp_string[0] == '{' and tmp_string[1] == '{':
		#最大範囲で{～}を取得
		pattern = r'\{(.*)\}\s*,?'
		m = re.search(pattern, tmp_string)
		#print(m.group(1))
		DivideElementBlock(m.group(1))
	else:
		#最小範囲で"{～}"を取得
		pattern = r'\{?(.*?)\}\s*?,?'
		l = re.finditer(pattern, tmp_string)
		#print("len=",len)
		for il in l:
			#m = re.search(pattern, il.group(0))
			#print("BBBB:"+m.group(1))
			g_lst_e.append(DivideElemenet(il.group(1)))
			len = len + 1
		if len == 0:
			g_lst_e.append(DivideElemenet(tmp_string))
	return g_lst_e

#-------------------------------------------------------------
# メイン処理
#-------------------------------------------------------------

# numpy arrayのprint出力設定
#print(np.get_printoptions())
np.set_printoptions(linewidth=sys.maxsize, threshold=sys.maxsize)

# デコード配列情報
g_lst_e = list()

tmp_string1 = GetInputString()
if tmp_string1 == '':
	exit

# 行コメント削除 否定先読み(https://www.javadrive.jp/python/regex/index16.html)
tmp_string2 = re.sub(r'//(?!\*).*', '', tmp_string1)
# 改行コード削除
tmp_string3 = tmp_string2.replace("\n", "")
# 複数行コメント削除(TODO入れ子のコメントには未対応)
tmp_string4= re.sub(r'#if 0.*?#endif', '', tmp_string3)
# 複数行コメント削除(TODO入れ子のコメントには未対応)
input_string = re.sub(r'/\*.*?\*/', '', tmp_string4)

# DEBUG用
#print("[1]", tmp_string1)
#print("[2]", tmp_string2)
#print("[3]", tmp_string3)
#print("[4]", tmp_string4)
#print("[5]", input_string)

# 構造体抽出
# ";"で分割する
list_struct = ExtractStruct(input_string)

for l in list_struct:
	# 構造体要素抽出
	match_struct = ExtractStructCont(l)
	if match_struct == None:
		continue
	if match_struct.lastindex != 3:
		continue
	# 配列要素分解
	DivideElementBlock(match_struct.group(3))

	print("name = ", match_struct.group(1))
	#print("n_index = "+match_struct.group(2))

	# DEBUG用：numpy arrny への変換に失敗した場合の確認用
	#print("list size = ", [len(value) for value in g_lst_e])
	
	# numpy arrayに変換する
	np_array = np.array(g_lst_e)
	print("ndim = ", np_array.shape)
	print()
	print(np_array)
	print()
	print("--------------------------------------------------------------------------------")
	g_lst_e.clear()

