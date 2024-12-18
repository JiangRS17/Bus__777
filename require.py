import boto3
import json
import subprocess
import re
from openai import OpenAI
from pathlib import Path
import os
import socket
import threading

import numpy as np
import pandas as pd
import requests
import psycopg2
from text2vec import SentenceModel 
from psycopg2.extensions import register_adapter, AsIs
psycopg2.extensions.register_adapter(np.float32, psycopg2._psycopg.AsIs)

# Step 1: User Input (Natural Language Description)
def get_user_input():
    # print("Please describe your application scenario:")
    user_input = input("Please describe your application scenario:")
    
    # 输出用户输入的内容
    # print(f"你输入的内容是: {user_input}")
    
    return user_input

# Step 2: GPT Analysis Function
client = OpenAI(
    api_key="sk-jxGxEstKGNXlwD4gaFIIveFQOAWm7hRxUZth8k191o5m8DE4",
    base_url="https://a.fe8.cn/v1"
)

def gpt_process_code(prompt):
    response = client.chat.completions.create(
        model="gpt-4",
        # -----升级
        messages=[{"role": "user", "content": f"{prompt}\n\n."}],
        stream=False,
    )
    temperature=0.7
    n = 3
    # max_tokens=64,
    # top_p=1
  
    
    solutions = []
    if response.choices:
        for choice in response.choices:
            solutions.append(choice.message.content.strip())
    else:
        solutions.append("No suggestion found.")

    return solutions


def extract_key_value_pairs(text):
    result = []
    paragraphs = text.strip().split("\n\n")
    for paragraph in paragraphs:
        if ":" in paragraph:
            # 提取冒号前的键和值
            key, value = paragraph.split(":", 1)
            key = key.strip()  # 去掉多余空格
            value = value.strip().replace("\n", " ")  # 替换换行符为单个空格
            result.append(f"{key}：{value}；")
    return result


def analyze_scenario(user_input):
    # print("prompt..........")
    prompt = "You are an expert in operating system design. Your task is to analyze the operating system design scenario I send you, \
                    identify the key characteristics of that scenario, and generate a corresponding operating system solution based on those features.\
                 Requirements:\
                 1.Architecture:[Architecture:Describe architecture, e.g., microkernel, monolithic, hybrid].\
                 2.Components: [List necessary components, e.g., scheduling, memory management, device drivers].\
                 3.Implementation Strategies:According to the overall design of the soft bus, components should be placed at \
                     which layer of the system based on the selected system architecture for explanation. \
                    Note: Components placed on the same layer of soft bus have the same privilege level\
                 Represent in the following format :[SoftBus_1: component x ,y,z; ].\
                 4. Let us think step by step.\
                 "
    response = gpt_process_code(prompt)
    # print(response)
    # return response
    # 将 response 转换为字符串
    if isinstance(response, list):
        response = "\n".join(response)  # 将列表元素合并为单个字符串
    formatted_response = extract_key_value_pairs(response)
    
    # 打印和返回结果
    for item in formatted_response:
        print(item)
    return formatted_response

# Step 3: Component Extraction Function (Post-Processing GPT Output)
def extract_components(gpt_output):
    print(".....step3......")
    components = []
    # 检查 gpt_output 的类型
    if isinstance(gpt_output, list):
        # 将列表中的元素拼接成一个长字符串，用换行符分隔
        gpt_output = "\n".join(gpt_output)
    elif not isinstance(gpt_output, str):
        # 如果 gpt_output 既不是字符串也不是列表，抛出异常
        raise TypeError("gpt_output 应该是字符串或包含字符串的列表")

    # 按行分割
    lines = gpt_output.split("\n")
    
    # 遍历每一行
    for line in lines:
        if ":" in line:
            try:
                # 按冒号分割，最多分割一次
                component, description = line.split(":", 1)
                components.append({
                    "componentType": component.strip(),
                    "freq": "UNKNOWN",  # Placeholder for frequency
                    "funCode": description.strip()
                })
            except ValueError:
                # 如果分割失败，跳过该行
                continue
    
    print(components)
    return components

# Step 4: Generate Output
def display_components(components):
    print("\nGenerated Operating System Design Components:\n")
    for component in components:
        print(f"- ComponentType: {component['componentType']}, ExchangeFreq: {component['freq']}, Message: {component['funCode']}")

#add sql support
## get embeddings
def get_embeddings(content):
    model = SentenceModel('shibing624/text2vec-base-chinese')
    sentence = content          # 返回 NumPy 数组
    vec = model.encode(sentence)

    return vec

##数据库插入tag对应向量
def Updata_embedding(embedding_tag,id):
    conn = psycopg2.connect(dbname='rustlathe',user='postgres',password='616717',host='localhost',port='5432')
    cur = conn.cursor()
    cur.execute("""
        UPDATE error_recores
        SET embedding = %s
        WHERE id = %s
    """,  (embedding_tag,id))
    
    conn.commit()
    cur.close()
    conn.close()
    

# # 计算相似度
def find_similar_errors(embedding):
    conn = psycopg2.connect(dbname='rustlathe',user='postgres',password='616717',host='localhost',port='5432')
    cur = conn.cursor()
    
    # 将 NumPy 数组转换为列表并格式化为 PostgreSQL VECTOR 类型的字符串
    embedding_str = f"[{', '.join(map(str, embedding.tolist()))}]"
    
    # WHERE(embedding <-> %s::vector) >= 0.5
    cur.execute(
        """
        SELECT example_code, soultion, soultion_code 
        FROM error_recores
        ORDER BY embedding <-> %s::vector
        LIMIT 1;
        """, (embedding_str,))
    res = cur.fetchall()
    result = {}
    for i, (example_code, soultion, soultion_code) in enumerate(res,1):
        result[f'Prompt{i}'] = {'example_code':example_code, 'solution':soultion, 'soultion_code':soultion_code}
    cur.close()
    conn.close()
    print(result)
    return result



# Main Execution Flow
def main():
    user_input = get_user_input()
    # print("......step 1.........")
    # print(user_input)
    gpt_output = analyze_scenario(user_input)
    components = extract_components(gpt_output)
    display_components(components)
    
if __name__ == "__main__":
    main()