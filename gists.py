import os
import requests
import json
import toml
from pathlib import Path

def create_header():
    header = { 'Accept': 'application/vnd.github.v3+json' }
    return header

# Print
def print_response(r):
    print(f"Request Status Code     {r.status_code}")
    print(f"Request Content Type    {r.headers['content-type']}")
    print(f"Request Encoding        {r.encoding}")
    print(f"Request Body            {r.text}")
    print(f"Request Json            {r.json()}")
    print(f"Request Content         {r.content}")

def list_packages(r, pkg_list):
    print(f"Package List: {pkg_list}")
    print(r.text)

def pretty_print(r, json_file=False):
# def pretty_print(gist, json_file=False):
    if (json_file):
        parsed_response = json.loads(r.text)
        print(json.dumps(parsed_response, indent=4, sort_keys=True))
    else:
        print_response(r)

# File
def read_file(filename):
    home = str(Path.home())
    file = open(home + "/" + filename, 'r')
    try:
        token = (toml.load(file)["user"]).get("token")
        print(f"Token: {token}")
        return token
    finally:
        file.close()

def create_json(local_name, gist_name, desc, contents, public=False):
# def create_json(gist_name, desc, contents, public=False):
    data = { 'description': desc, 'public': public }
    data['files'] = {}
    data['files'][gist_name] = { 'content': contents }
    result = json.dumps(data)
    print(result)
    return result
    # with open(local_name, 'w') as output:
    with open(local_name, 'w') as output:
        json.dump(data, output)

# Request
def send_get(url, token):
    r   = requests.get(url, allow_redirects=True, headers=create_header(),
                        auth=('Authentication', token))
    return r

# def send_post(url, token, filename):
def send_post(url, token, filename):
    contents = open(filename, 'rb').read()
    r   = requests.post(url, allow_redirects=True, headers=create_header(),
                        auth=('Authentication', token),
                        data=contents)
    return r

# Gist
class Gist():
    def __init__(self, url, config, gist_name, desc):
        self.url = url
        self.config = config
        self.gist_name = gist_name
        self.desc = desc

    def create_gist(self, local_name, contents):
        # create_json(gist_name, desc, contents) 
        create_json(local_name, gist_name, desc, contents, public=False)
        token = read_file(config)
        return (send_post(url, token, local_name))

    def get_gists(self):
        token = read_file(config)
        # aur_list_pkgs, arch_list_pkgs = pretty_print(send_get(url, token), json_file=True)
        # return aur_list_pkgs, arch_list_pkgs = pretty_print(send_get(url, token), json_file=True) 
        r = send_get(url, token)
        parsed_response = json.loads(r.text)
        aur_list_pkgs   = parsed_response[0]['files']['aur-list.pkg']['raw_url']
        arch_list_pkgs  = parsed_response[1]['files']['pacman-list.pkg']['raw_url']
        print("Aur      Raw Url: {aur_list_pkgs} ")
        print("Pacman   Raw Url: {arch_list_pkgs}")
        # list_packages((send_get(aur_list_pkgs, token)), "Aur")
        # list_packages((send_get(arch_list_pkgs, token)), "Arch")

url = "https://api.github.com/gists"
config = ".config/gist/config.toml"

# token = read_file(config)
# print_response(send_post(token))
# send_post(token)
# create_json("TestFile.txt", "TestFile2.json", "Creating a test file via GitHub Gist API")
# 'TestFile.json'

local_name  = "TestFile2.json"
gist_name   = "TestFile2"
desc        = "Creating a test file via GitHub Gist API"
contents    = "This is a test file."
aur_gist    = Gist(url, config, gist_name, desc)
pretty_print(aur_gist.create_gist(local_name, contents), json_file=False)

# get_gists(url, config)



# print(f"Saving as: {directory}/{names.get()}.zip", flush=True)
# if (mini.status_code != 404):
    # writeToFile(mini.content, f"{directory}/{names.get()}.zip", 'wb')
