import re
import regex
import pprint
import csv
import os

str_decode = ""

def extract_braces(content, indices=[], max_depth=0, tbl_name="table-name"):
    """
    入れ子になったCスタイルの配列の構造を文字列コンテンツから抽出して表示します。
    この関数は、入力コンテンツ内の中括弧を再帰的に見つけて処理し、Cスタイルの配列を表現します。
    適切な次元とインデックスで配列の構造を表示します。
    引数:
        content (str): 入れ子になった中括弧を含むCスタイルの配列を含む文字列コンテンツ。
        indices (list, optional): 入れ子になった配列内の現在の位置を表すインデックスのリスト。
                                  デフォルトは空のリスト。
        max_depth (int, optional): 入れ子になった配列の最大深度。デフォルトは0。
    戻り値:
        なし
    例:
        入力コンテンツ "{ {1, 2}, {3, 4} }" が与えられた場合、関数は次を表示します:
        int a[0][0] = { 1, 2 }
        int a[0][1] = { 3, 4 }
    """

    global str_decode
    # 再帰的に中括弧を見つけるパターン
    inner_pattern = r'\{(?:[^{}]*|(?R))*\}'
    matches = regex.findall(inner_pattern, content)

    # 配列の次元数に応じて [] を決定
    dimensions = '[]' * (max_depth - len(indices))  # 残りの次元数に対応する []
    index_str = ''.join(f'[{i}]' for i in indices)  # 現在のインデックスを文字列として構成

    if matches == []:
        # 配列の要素を取得
        elements = content.split(',')
        # print(f"{tbl_name}{index_str}{dimensions} = {{ {', '.join(elements)} }}")  # 配列の要素を表示
        str_decode += f"{tbl_name}{index_str}{dimensions} = {{ {', '.join(elements)} }}\n"
        return

    # 余分な空白や改行を削除して内容を整形
    cleaned_content = content.strip().replace('\n', '').replace(' ', '')
    if cleaned_content.startswith('{') and cleaned_content.endswith('}'):
        cleaned_content = cleaned_content[1:-1]  # 最外側の中括弧を取り除く

    # 再帰的にさらに内側を探索
    for idx, match in enumerate(matches):
        extract_braces(match[1:-1], indices + [idx], max_depth, tbl_name)  # 次のインデックスを追加して再帰呼び出し

# データをネストされた辞書として挿入する関数
def insert_data(d, indices, values):
    for index in indices[:-1]:
        index = int(index)
        if index not in d:
            d[index] = {}
        d = d[index]
    d[int(indices[-1])] = values


def print_dict(data, tbl_name="AA"):
    """
    再帰的に辞書（多次元リストまたはネストされたリスト構造）を処理し、
    ターミナル出力を制御する関数。
    
    - 1次元の場合: カンマ区切りで出力
    - 2次元の場合: 行ごとにカンマ区切りで出力
    - 3次元以上の場合: 複数の2次元出力を改行で区切る
    """
    if isinstance(data[0], dict):
        for key, val in data:
            print_dict(val , tbl_name + [key])
    else:
        # 2次元リスト
        print(tbl_name)
        for item in data:
            print(", ".join(item.values()))
            print()


def parse_c_array_to_csv(input_file, output_file):
    """
    C言語で定義された多次元配列を解析し、CSVファイルに出力します。
    引数:
        input_file (str): C言語の配列定義を含むテキストファイル。
        output_file (str): 変換されたCSVファイルの出力先。
    この関数は、入力ファイルの内容を読み込み、正規表現を使用してコメントを削除し、
    最外の中括弧を抽出します。その後、再帰的に内側の中括弧を解析し、解析されたデータを
    CSVファイルに書き込みます。
    """
    global str_decode

    # 入力ファイルを開いて内容を読み込む
    with open(input_file, 'r', encoding='utf-8') as f:
        data = f.read()

    ###正規表現の参考
    # https://qiita.com/mcla/items/02fc249a18eb623bf100
    # 正規表現で配列の中身を抽出

    # コメントを含む行も対象にするために、コメントを除去する正規表現を追加
    data = re.sub(r'//.*?$|/\*.*?\*/', '', data, flags=re.DOTALL | re.MULTILINE)

    # ✅ 外側の中括弧を抽出するパターン（re使用）
    outer_pattern = r'(^.*?)((\[.*?\])+)\s*?=\s*?(\{.*?\})\s*?;'  # 最外側の中括弧を探すパターン
    # すべてのマッチを取得
    matches = re.findall(outer_pattern, data, flags=re.DOTALL | re.MULTILINE)

    # 再帰的にさらに内側を探索
    for idx, match in enumerate(matches):

        str_decode = ""

        # データを保持するためのネストされた辞書
        dict_data = {}

        tbl_name = match[0].strip() 
        max_depth = match[1].count('[')
        str_data = match[3].strip().replace(' ', '').replace('\t', '').replace('\n', '')

        # 再帰的に中括弧を解析
        extract_braces(str_data, max_depth=max_depth, tbl_name=tbl_name)

        print(str_decode)

                
        # 入力データから配列とその値を抽出するパターン
        pattern = r'(.*?)((?:\[\d+\])+)?\s*=\s*\{([^}]*)\}'

        # 正規表現で全てのマッチを探す
        str_decode_matches = re.findall(pattern, str_decode)

        # 各マッチに対して辞書にデータを挿入
        for str_decode_match in str_decode_matches:
            tblname_str, indices_str, values_str = str_decode_match
            # 'indices_str'が空でない場合、正規表現を使用して文字列からすべての数字を抽出し、リストとして返します。
            # 'indices_str'が空の場合、空のリストを返します。
            indices = re.findall(r'\d+', indices_str) if indices_str else []

            # 'values_str'をカンマで分割し、各要素の前後の空白を削除して整数に変換します。
            # 空白のみの要素は無視されます。
            values = [v.strip() for v in values_str.split(',') if v.strip()]

            if indices:
                insert_data(dict_data, indices, values)
            else:
                dict_data = values

        # 結果の表示
        #pprint.pprint(dict_data)
        
        # ★検討中★
        #print_dict(dict_data, tbl_name)

        # CSVファイルとして出力
        """
        with open(output_file, 'aw', newline='', encoding='utf-8') as csvfile:
            csvwriter = csv.writer(csvfile)
            csvwriter.writerows(parsed_data)  # CSVにデータを書き込み
        """

if __name__ == "__main__":
    print("Running as a script")

    # 使用例
    input_file = "c_array_decode/input.c"  # 入力ファイル名
    input_file = "c_array_decode/sample_table/c_d_otbl.c"
    input_file = "c_array_decode/sample_table/t1v_otbl.c"
    input_file = "c_array_decode/sample_table/srv_otbl.c"
    output_file = "c_array_decode/output.csv"  # 出力ファイル名
    parse_c_array_to_csv(input_file, output_file)
