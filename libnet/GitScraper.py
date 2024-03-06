from queue import Queue
import random
import string
import time
import json
import base64

import requests
from bs4 import BeautifulSoup

# Scraper for reading repository data from GitHub
class GitScraper:

    # The class is initialized with GitHub API OAuth2 token
    def __init__(self, token):
        self.token = token
        self.headers = {
                "Authorization": f"token {self.token}",
                "Accept" : "application/vnd.github.v3+json"
        }
        self.rate_check_url = "https://api.github.com/rate_limit"
        self.repo_search_url = "https://api.github.com/search/repositories?"
        self.file_search_url = "https://api.github.com/search/code?"
        self.repo_queries_remaining = 30
        self.file_queries_remaining = 5000

    # Get the BeautifulSoup DOM of a website
    def get_bs_dom(self, url):
        res = requests.get(url, headers={'User-Agent': "Mozilla/5.0"})
        if res.status_code != 200:
            print(f"Accessing {url} failed")
            return
        else:
            return BeautifulSoup(res.content, "html.parser")

    def get_repos_from_dom(self, dom):
        repo_elems = dom.select("article.Box-row h1")
        repos = Queue()

        for e in repo_elems:
            href = e.a.attrs["href"]
            repos.put(href.lower()[1:])

        return repos

    # Helper for checking the amount of API queries remaining
    def api_check_remaining(self):
        print(f"Remaining repo queries: {self.repo_queries_remaining} | Remaining file queries: {self.file_queries_remaining}")
        return min(self.file_queries_remaining, self.repo_queries_remaining)

    # Get repository search results as a json
    def get_search_json(self, q_type, search_url):
        remaining = self.api_check_remaining()
        if remaining == 0:
            print("Out of requests! Sleeping until reset...")
        while remaining == 0:
            time.sleep(1)
            rate_res = requests.get(self.rate_check_url, headers=self.headers) 
            self.repo_queries_remaining = int(rate_res.json()["resources"]["search"]["remaining"])
            remaining = self.repo_queries_remaining

        res = requests.get(search_url, headers=self.headers)

        if q_type == "repo":
            self.repo_queries_remaining = int(res.headers["X-Ratelimit-Remaining"])
        elif q_type == "file":
            self.file_queries_remaining = int(res.headers["X-Ratelimit-Remaining"])
        else:
            print(f"Invalid q_type: {q_type}")
            return

        if res.status_code != 200:
            print(f"Accessing {search_url} failed")
            return
        else:
            return res.json()

    # Parser for requirements.txt files
    def read_requirements_file(self, url):
        res = self.get_search_json("file", url)
        reqs_decoded = str(base64.b64decode(res["content"]), "utf-8")
        reqs_rows = reqs_decoded.split('\n')
        reqs = []
        for r in reqs_rows:
            if len(r) > 0:
                ln = r
                if ln[0].isalpha:
                    gt = ln.find('>') # Greater than
                    eq = ln.find('=') # Equal
                    ws = ln.find(' ') # Whitespace
                    if gt > 0:
                        ln = ln[:gt]
                    elif eq > 0:
                        ln = ln[:eq]
                    elif ws > 0:
                        ln = ln[:ws]

                    ln.strip()
                    if ln[0].isalpha() and len(ln) > 0 and not ln.startswith("git+"):
                        reqs.append(ln.lower())
        return reqs

    # Get the list of requirements/dependencies of a repository from its requirements.txt file
    def get_requirements_from_search(self, repo_full):
        url = self.file_search_url
        q = f"q=filename:requirements+repo:{repo_full}"
        search_results = self.get_search_json("repo", url + q)

        repo_requirements = []
        for item in search_results["items"]:
            name = item["name"]
            url = item["git_url"]
            if ".txt" in name:
                repo_requirements.extend(self.read_requirements_file(url))
        
        return repo_requirements

    # Get the name of a repository from repository search whose name corresponds exactly to the
    # one given as parameter (i.e. returns the name of the repository if it exists)
    def get_repo_from_search(self, repo):
        url = self.repo_search_url
        q = f"q={repo}&language=Python&per_page=10"
        search_results = self.get_search_json("repo", url + q)

        for item in search_results["items"]:
            name = item["full_name"]
            lib_name = item["name"]
            if lib_name.lower() == repo.lower():
                return name.lower()

    def get_filler_repos(self):
        url = self.repo_search_url
        # Hack for searching for "random" python repositories
        rand = chr(random.randrange(97, 122))
        q = f"q={rand}&language=Python&per_page=15"
        search_results = self.get_search_json("repo", url + q)
        repos = Queue()
        for item in search_results["items"]:
            name = item["full_name"]
            repos.put(name)

        return repos
            


