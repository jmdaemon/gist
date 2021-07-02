import os
import requests
import json
import toml
from pathlib import Path

# Print
def print_response(res):
    print(f"Request Status Code     {res.status_code}")
    print(f"Request Content Type    {res.headers['content-type']}")
    print(f"Request Encoding        {res.encoding}")
    print(f"Request Body            {res.text}")
    print(f"Request Json            {res.json()}")
    print(f"Request Content         {res.content}")

def list_packages(res, pkg_list):
    print(f"Package List: {pkg_list}")
    print(res.text)

def pretty_print(res, display_json=False):
    if (display_json):
        parsed_response = json.loads(res.text)
        print(json.dumps(parsed_response, indent=4, sort_keys=True))
    else:
        print_response(res)

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
    data = { 'description': desc, 'public': public }
    data['files'] = {}
    data['files'][gist_name] = { 'content': contents }
    result = json.dumps(data)
    print(result)
    return result
    with open(local_name, 'w') as output:
        json.dump(data, output)

# Request
def create_header():
    header = { 'Accept': 'application/vnd.github.v3+json' }
    return header

def send_get(url, token):
    r   = requests.get(url, allow_redirects=True, headers=create_header(),
                        auth=('Authentication', token))
    return r

def send_post(url, token, filename):
    contents = open(filename, 'rb').read()
    r   = requests.post(url, allow_redirects=True, headers=create_header(),
                        auth=('Authentication', token),
                        data=contents)
    return r

# Gist
class Gist():
    def __init__(self, url, config, gist_name, desc, public):
        self.url        = url
        self.token      = read_file(config)
        self.config     = config
        self.gist_name  = gist_name
        self.desc       = desc
        self.public     = public

    def list_gists(self, display_json):
        pretty_print(get_gists(self), display_json)

    def create_gist(self, local_name, contents):
        create_json(local_name, self.gist_name, self.desc, contents, self.public)
        res = send_post(self.url, self.token, local_name)
        return res

    def get_gists(self):
        res = send_get(self.url, self.token)
        return res


url = "https://api.github.com/gists"
config = ".config/gist/config.toml"

# == Create User Gists ==
# local_name  = "TestFile2.json"
# gist_name   = "TestFile2"
# desc        = "Creating a test file via GitHub Gist API"
# contents    = "This is a test file."
# public      = False
# aur_gist    = Gist(url, config, gist_name, desc, public)

# res = aur_gist.create_gist(local_name, contents)
# pretty_print(res, display_json=True)
# print(f"Gist: {gist_name} Created!")

# == Get User Gists ==

json_response   = send_get(url, read_file(config)).json()
aur_list_pkgs   = json_response[0]['files']['aur-list.pkg']['raw_url']
arch_list_pkgs  = json_response[1]['files']['pacman-list.pkg']['raw_url']
print(f"Aur    Raw: {aur_list_pkgs} ")
print(f"Pacman Raw: {arch_list_pkgs}")
# list_packages((send_get(aur_list_pkgs, token)), "Aur")
# list_packages((send_get(arch_list_pkgs, token)), "Arch")
