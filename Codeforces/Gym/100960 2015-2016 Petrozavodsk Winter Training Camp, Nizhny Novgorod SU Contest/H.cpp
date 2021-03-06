#include <cstdio>
#include <vector>
using namespace std;
#define INF (1<<30)

struct node
{   int size, id, w;
    node *par, *ppar, *left, *right;
    node() {
		par = ppar = left = right = NULL; 
		w = size = INF;
	}
};

class LinkCutTree
{
	vector<node> lct;

	void update(node* p) {
		p->size = p->w;
		if (p->left) p->size += p->left->size;
		if (p->right) p->size += p->right->size;
	}

	void rotateright(node* p) {
		node *q, *r;
		q = p->par, r = q->par;
		if ((q->left = p->right)) q->left->par = q;
		p->right = q, q->par = p;
		if ((p->par = r)) {
			if (q == r->left) r->left = p;
			else r->right = p;
		}
		p->ppar = q->ppar;
		q->ppar = NULL;
		update(q);
	}

	void rotateleft(node* p) {
		node *q, *r;
		q = p->par, r = q->par;
		if ((q->right = p->left)) q->right->par = q;
		p->left = q, q->par = p;
		if ((p->par = r))
		{
			if (q == r->left) r->left = p;
			else r->right = p;
		}
		p->ppar = q->ppar;
		q->ppar = 0;
		update(q);
	}

	void splay(node* p) {
		node *q, *r;
		while (p->par != NULL) {
			q = p->par;
			if (q->par == NULL) {
				if (p == q->left) rotateright(p);
				else rotateleft(p);
			}
			else {
				r = q->par;
				if (q == r->left) {
					if (p == q->left) rotateright(q), rotateright(p);
					else rotateleft(p), rotateright(p);
				}
				else {
					if (p == q->right) rotateleft(q), rotateleft(p);
					else rotateright(p), rotateleft(p);
				}
			}
		}
		update(p);
	}

	node* access(node* p){
		splay(p);
		if (p->right != NULL) {
			p->right->ppar = p;
			p->right->par = NULL;
			p->right = NULL;
			update(p);
		}
		node* last = p;
		while (p->ppar != NULL) {
			node* q = p->ppar;
			last = q;
			splay(q);
			if (q->right != NULL) {
				q->right->ppar = q;
				q->right->par = NULL;
			}
			q->right = p;
			p->par = q;
			p->ppar = NULL;
			update(q);
			splay(p);
		}
		return last;
	}

public:
    LinkCutTree() { }
    LinkCutTree(int n) {
		lct.resize(n + 1);
        for(int i = 0; i <= n; i++){
			lct[i].id = i;
            update(&lct[i]);
        }
    }

	void link(int u, int v, int w) { //u becomes child of v
		node *p = &lct[u], *q = &lct[v];
		access(p);
		access(q);
		p->left = q;
		q->par = p;
		p->w = w;
		update(p);
	}

	void link(int u, int v) { //unweighted
		link(u, v, 1);
	}

    void cut(int u) {
		node* p = &lct[u];
		access(p);
		p->left->par = NULL;
		p->left = NULL;
		update(p);
    }

    int findroot(int u) {
		node* p = &lct[u];
		access(p);
		while (p->left) p = p->left;
		splay(p);
		return p->id;
    }

	bool IsSameTree(int u, int v) {
		return findroot(u) == findroot(v);
	}

    int depth(int u) {
		access(&lct[u]);
		return lct[u].size - lct[u].w;
    }

    int LCA(int u, int v) {
		access(&lct[u]);
		return access(&lct[v])->id;
    }
};

class UndirectedLinkCutTree
{
	LinkCutTree lct;
	vector<int> par;

	void invert(int u) {
		if (par[u] == -1) return;
		int v = par[u];
		invert(v);
		lct.cut(u);
		par[u] = -1;
		lct.link(v, u);
		par[v] = u;
	}
public:
	UndirectedLinkCutTree() {}
	UndirectedLinkCutTree(int n) {
		lct = LinkCutTree(n);
		par.assign(n+1, -1);
	}
	void link(int u, int v) {
		if (lct.depth(u) < lct.depth(v)) {
			invert(u);
			lct.link(u, v);
			par[u] = v;
		}
		else{
			invert(v);
			lct.link(v, u);
			par[v] = u;
		}
	}
	void cut(int u, int v) {
		if (par[v] == u) u = v;
		lct.cut(u);
		par[u] = -1;
	}
	bool IsSameTree(int u, int v) {
		return lct.IsSameTree(u, v);
	}
};

/*
 * Codeforces 100960H
 */

UndirectedLinkCutTree ulct;

int main()
{
	int N, a, b;
	char op;
	scanf("%d", &N);
	ulct = UndirectedLinkCutTree(N);
	while(true) {
		scanf(" %c", &op);
		switch(op) {
		case 'C':
			scanf("%d %d", &a, &b);
			ulct.link(a, b);
			break;
		case 'D':
			scanf("%d %d", &a, &b);
			ulct.cut(a, b);
			break;
		case 'T':
			scanf("%d %d", &a, &b);
			if (ulct.IsSameTree(a, b)) printf("YES\n");
			else printf("NO\n");
			fflush(stdout);
			break;
		case 'E':
			return 0;
		}
	}
	return 0;
}