#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef struct {
	int Obj1;
	int Obj2;
} Result;

typedef struct {
	int k1;
	int k2;
	int k3;
} K;

typedef struct {
	int* res1;
	int* res2;
} KK;
typedef struct {
	int* Archive;
	int* Archive_cost;
}domdel;

void AMOSA(Archive,Archive_Len,solution_Len,Archive_cost,iter, CG, new_pt, Q, L,
	number_Of_vehicle, cost_demand, genetic_Form_key,genetic_Form_value,temp,T_min) {
    current_pt = (int*)malloc(solution_Len * sizeof(int));
    current_pt_cost = (int*)malloc(solution_Len * sizeof(int));
    if (Archive_Len > 1){
        int random_number = rand() % Archive_Len;
        for (int i = 0; i < solution_Len; i++;) {
            current_pt[i] = Archive[random_number][i];
            current_pt_cost[i] = Archive_cost[random_number][i];
        }
    else {
        for (int i = 0; i < solution_Len; i++;) {
            current_pt[i] = Archive[0][i];
            current_pt_cost[i] = Archive_cost[0][i];
        }
    }

    while (temp > T_min) {
        for (int i = 0; i < iter; i++) {
            new_pt = neighborhood_Generator(current_pt);
			new_pt_cost = Split(CG, new_pt, Q, L, number_Of_vehicle, cost_demand, genetic_Form);
			if (new_pt_cost[0] == NULL) {
				continue;
			}
			int current1, current2, new1, new2;
			current1 = current_pt_cost->Obj1;
			current2 = current_pt_cost->Obj2;
			new1 = new_pt_cost->Obj1;
			new2 = new_pt_cost->Obj2;
			dom = pareto_Dominance(current1, current2, new1, new2);
			if (dom == 0) {
				int dom_avg = 0;
				KK* result = find_k(Archive, Archive_cost, new_pt_cost, Archive_Len);
				K* ress1 = result->res1;
				K* ress2 = result->res2;
				int k = ress1->k1;
				int k_cost = ress2->k1;
				int dom_avg = 0;
				int len_k = 0;
				for (j = 0; j < Archive_Len; j++) {
					if (k_cost[j] != -1) {
						len_k++;
						int a_cost1, a_cost2, b_cost1, b_cost2;
						a_cost1 = k_cost[j];
						a_cost2 = k_cost[2 * j];
						b_cost1 = new_pt_cost[0];
						b_cost2 = new_pt_cost[1];
						dom_avg += dom_calculator(a_cost1,a_cost2,b_cost1,b_cost2,R1,R2);
					}
				}
				int a_cost1, a_cost2, b_cost1, b_cost2;
				a_cost1 = current_pt_cost[0];
				a_cost2 = current_pt_cost[0];
				b_cost1 = new_pt_cost[0];
				b_cost2 = new_pt_cost[0];
				dom_avg += dom_calculator(a_cost1, a_cost2, b_cost1, b_cost2, R1, R2);
				float dom_avgg;
				dom_avgg = dom_avg / (1.0 + len_k);
				float prob;
				prob = 1.0 / (1 + exp(dom_avgg*temp));
				float random = (float)rand()/RAND_MAX;
				if (prob > random) {
					current_pt = new_pt;
					current_pt_cost = new_pt_cost;
				}
			}
			else if (dom == 1) {
				KK* result = find_k(Archive, Archive_cost, new_pt_cost, Archive_Len);
				K* ress1 = result->res1;
				K* ress2 = result->res2;
				bool flag = false;
				int k = ress1->k1;
				int k_cost = ress2->k1;
				int dom_avg = 0;
				int len_k = 0;
				for (int j = 0; j < 2 * Archive_Len; j++) {
					if (k[j] != -1) {
						flag = true;
						break;
					}
				}
				if (flag) {
					for (j = 0; j < Archive_Len; j++) {
						if (k_cost[j] != -1) {
							len_k++;
							int a_cost1, a_cost2, b_cost1, b_cost2;
							a_cost1 = k_cost[j];
							a_cost2 = k_cost[2 * j];
							b_cost1 = new_pt_cost[0];
							b_cost2 = new_pt_cost[1];
							dom_avg += dom_calculator(a_cost1, a_cost2, b_cost1, b_cost2, R1, R2);
						}
					}
					float dom_avgg;
					dom_avgg = dom_avg / (float)len_k;
					float prob;
					prob = 1.0 / (1 + exp(dom_avgg * temp));
					float random = (float)rand() / RAND_MAX;
					if (prob > random) {
						current_pt = new_pt;
						current_pt_cost = new_pt_cost;
					}
				}
				else {
					flag = true;
					int k = ress1->k2;
					int k_cost = ress2->k2;
					for (int j = 0; j < 2 * Archive_Len; j++) {
						if (k[j] == -1) {
							flag = false;
							break;
						}
					}
					if (flag) {
						current_pt = new_pt;
						current_pt_cost = new_pt_cost;
						for (int j = 0; j < Archive_Len; j++) {
							if (Archive[j] == -1) {
								Archive[j] = new_pt;
								// Archive cost 2배로 늘려서 하나씩 저장할 예정
								Archive_cost[j] = new_pt_cost;
								break;
							}
						}
						
					}
					else {
						//dominance에서 동일할 때 예시 삭제하고 아래 if 빼고 적을 예정
						flag = false;
						int k = ress1->k3;
						int k_cost = ress2->k3;
						for (int j = 0; j < 2 * Archive_Len; j++) {
							if (k[j] != -1) {
								flag = true;
								break;
							}
						}
						if (flag) {
							current_pt = new_pt;
							//cost도 변수 두개로 나눠서 저장예정
							current_pt_cost = new_pt_cost;
							for (int j = 0; j < Archive_Len; j++) {
								if (Archive[j] == -1) {
									Archive[j] = new_pt;
									// Archive cost 2배로 늘려서 하나씩 저장할 예정
									Archive_cost[j] = new_pt_cost;
									break;
								}
							}
						}
					}

				}
			}
			else if (dom == 2) {
				KK* result = find_k(Archive, Archive_cost, new_pt_cost, Archive_Len);
				K* ress1 = result->res1;
				K* ress2 = result->res2;
				bool flag = false;
				int k = ress1->k1;
				int k_cost = ress2->k1;
				// current pt로 바꿀 수 있는 변수
				int dom_min;
				dom_min = (int*)malloc(solution_Len * sizeof(int));
					
				int minimum = 10000000;
				int dom_value;
				int dom_min_cost;
				int len_k = 0;
				for (int j = 0; j < 2 * Archive_Len; j++) {
					len_k++;
					if (k[j] != -1) {
						flag = true;
					}
				}
				if (flag) {
					for (int j = 0; j < len_k; j++) {
						int a_cost1, a_cost2, b_cost1, b_cost2;
						a_cost1 = k_cost[j][0];
						a_cost2 = k_cost[j][1];
						b_cost1 = new_pt_cost[0];
						b_cost2 = new_pt_cost[1];
						dom_value = dom_calculator(a_cost1, a_cost2, b_cost1, b_cost2);
						if (minimum > dom_value) {
							dom_min = k[j];
							// 두개로 설정해서 cost 하나씩
							dom_min_cost = k_cost[j];
							minimum = dom_value;
						}
						float prob;
						prob = 1.0 / (1 + exp(-minimum));
						float random = (float)rand() / RAND_MAX;
						if (prob > random) {
							current_pt = dom_min;
							current_pt_cost = dom_min_cost;
						}
						else {
							current_pt = new_pt;
							current_pt_cost = new_pt_cost;
						}

					}
				}
				else {
					flag = true;
					int k = ress1->k2;
					int k_cost = ress2->k2;
					for (int j = 0; j < 2 * Archive_Len; j++) {
						if (k[j] == -1) {
							flag = false;
							break;
						}
					}
					if (flag) {
						
						for (int j = 0; j < Archive_Len; j++) {
							if (Archive[j] == -1) {
								Archive[j] = new_pt;
								Archive_cost[j] = new_pt_cost;
							}
							if (Archive[j] == current_pt) {
								//Archiv 변할때 ARchive len도 변하게끔 수정
								Archive[j] = -1;
								Archive_cost[j] = -1;
							}
						}
						current_pt = new_pt;
						current_pt_cost = new_pt_cost;
					else {
						//dominance에서 동일할 때 예시 삭제하고 아래 if 빼고 적을 예정
						flag = false;
						int k = ress1->k3;
						int k_cost = ress2->k3;
						for (int j = 0; j < 2 * Archive_Len; j++) {
							if (k[j] != -1) {
								flag = true;
								break;
							}
						}
						if (flag) {
							current_pt = new_pt;
							//cost도 변수 두개로 나눠서 저장예정
							current_pt_cost = new_pt_cost;
							for (int j = 0; j < Archive_Len; j++) {
								if (Archive[j] == -1) {
									Archive[j] = new_pt;
									// Archive cost 2배로 늘려서 하나씩 저장할 예정
									Archive_cost[j] = new_pt_cost;
									break;
								}
							}
						}
						domdel* res;
						res = dominance_Delete(Archive, Archive_cost, Archive_Len);
						Archive = res->Archvie;
						Archive_cost = res->Archive_cost;
					}
				}

			}

		}
    }
	iterrr++;
	printf("Cooling %d %d", temp, iterrr);
	for (int i = 0; i < Archive_Len; i++) {
		printf("%d ", Archive_cost[i]);
	}
	printf("\n");
	temp = alpha * temp;
}

int neighborhood_Generator(S) {
    int i,j,temp;
    while (i == j) {
        i = rand() % solution_Len;
        j = rand() % solution_Len;
    }
    temp = S[i];
    S[i] = S[j];
    S[j] = temp;

    return S;

}

Result* Split(G, S, Q, L, number_Of_vehicle, cost_demand, genetic_Form_key, genetic_Form_value, solution_Len) {
	int* Obj1;
	int* Arc1;
	int* Obj;
	int* Arc;
	int tour_inv[2] = { 0 };
	int tour_[2] = { 0 };
	int Obj1_len = solution_Len * 2 + 2;
	int Obj_len = solution_Len + 1;

	Obj1 = (int*)malloc(Obj1_len * sizeof(int));
	Arc1 = (int*)malloc(Obj1_len * sizeof(int));
	Obj = (int*)malloc(Obj_len * sizeof(int));
	Arc = (int*)malloc(Obj_len * sizeof(int));
	if (Obj == NULL) {
		printf("메모리 할당 오류\n");
		return 0;  // 프로그램 종료

	if (Obj1 == NULL) {
		printf("메모리 할당 오류\n");
		return 0;  // 프로그램 종료
	}
	if (Arc == NULL) {
		printf("메모리 할당 오류\n");
		return 0;  // 프로그램 종료
	}
	if (Arc1 == NULL) {
		printf("메모리 할당 오류\n");
		return 0;  // 프로그램 종료
	}
	for (int i = 0; i < solution_Len * 2 + 2; i++) {
		Obj1[i] = 1000000;
		Arc1[i] = 0;
	}
	Obj1[0] = 0;


	for (int i = 1; i < 1 + solution_Len; i++) {
		int demand1, demand2, cost1, cost2, j;
		demand1 = 0;
		demand2 = 0;
		cost1 = 0;
		cost2 = 0;
		j = i;
		while (j <= solution_Len && demand1 <= Q) {
			if (i == j) {

				int idx = linear_search(genetic_Form_key, solution_Len, S[j - 1]);
				int Sol[2] = { 0 };
				Sol[0] = genetic_Form_value[idx][0];
				Sol[1] = genetic_Form_value[idx][1];
				int demand = demand_(Sol[0], Sol[1], cost_demand);
				demand1 += demand;
				result = dijkstra(G, 1, Sol[0]);
				cost1 += result->distance;
				int demand_cost = cost_(Sol[0], Sol[1], cost_demand);
				cost1 += demand_cost;
				result = dijkstra(G, Sol[1], 1);
				cost1 += result->distance;

				int inv_S = _inverse(S[j - 1], solution_Len);
				int idx = linear_search(genetic_Form_key, solution_Len, inv_S);
				int Sol[2] = { 0 };
				Sol[0] = genetic_Form_value[idx][0];
				Sol[1] = genetic_Form_value[idx][1];
				int demand = demand_(Sol[0], Sol[1], cost_demand);
				demand2 += demand;
				result = dijkstra(G, 1, Sol[0]);
				cost2 += result->distance;
				int demand_cost = cost_(Sol[0], Sol[1], cost_demand);
				cost2 += demand_cost;
				result = dijkstra(G, Sol[1], 1);
				cost2 += result->distance;
			}
			else {
				int idx1 = linear_search(genetic_Form_key, solution_Len, S[j - 1]);
				int idx2 = linear_search(genetic_Form_key, solution_Len, S[j - 2]);
				int inv_S1 = _inverse(S[j - 1], solution_Len);
				int inv_S2 = _inverse(S[j - 2], solution_Len);
				int idx3 = linear_search(genetic_Form_key, solution_Len, inv_S1);
				int idx4 = linear_search(genetic_Form_key, solution_Len, inv_S2);
				int Sol[2] = { 0 };
				int _Sol[2] = { 0 };
				int inv_Sol[2] = { 0 };
				int _inv_Sol[2] = { 0 };

				Sol[0] = genetic_Form_value[idx1][0];
				Sol[1] = genetic_Form_value[idx1][1];

				_Sol[0] = genetic_Form_value[idx2][0];
				_Sol[1] = genetic_Form_value[idx2][1];

				inv_Sol[0] = genetic_Form_value[idx3][0];
				inv_Sol[1] = genetic_Form_value[idx3][1];

				_inv_Sol[0] = genetic_Form_value[idx4][0];
				_inv_Sol[1] = genetic_Form_value[idx4][1];

				int demand = demand_(Sol[0], Sol[1], cost_demand);
				demand1 += demand;
				demand2 += demand;
				demand_cost = cost_(Sol[0], Sol[1], cost_demand);
				result = dijkstra(_Sol[1], 1);
				cost1 -= result->distance;
				result = dijkstra(_inv_Sol[1], 1);
				cost2 -= result->distance;

				result = dijkstra(_Sol[1], Sol[0]);
				int _cost1 = result->distance;
				result = dijkstra(_Sol[1], inv_Sol[0]);
				int _cost2 = result->distance;
				result = dijkstra(_inv_Sol[1], Sol[0]);
				int _cost3 = result->distance;
				result = dijkstra(_inv_Sol[1], inv_Sol[0]);
				int _cost4 = result->distance;

				if (_cost1 > _cost3) {
					cost1 += _cost3;
				}
				else {
					cost1 += _cost1;
				}
				if (_cost2 > _cost4) {
					cost2 += _cost4;
				}
				else {
					cost2 += _cost2;
				}
				result = dijkstra(Sol[1], 1);
				cost1 += result->distance;

				result = dijkstra(inv_Sol[1], 1);
				cost2 += result->distance;

				if (demand1 < Q) {
					int idx1 = 2 * (i - 1);
					int idx2 = 2 * (i - 1) + 1;
					int idx3 = 2 * j;
					int idx4 = 2 * j + 1;
					int New1 = cost1 + Obj1[idx1];
					int New2 = cost2 + Obj1[idx2];

					if ((New1 < Obj1[idx3]) || ((New1 == Obj1[idx3]) && (Arc1[idx1] + 1 < Arc1[idx3]))) {
						Obj1[idx3] = New1;
						Arc1[idx3] = Arc[idx1] + 1;
					}
					if ((New1 < Obj1[idx4]) || ((New1 == Obj1[idx4]) && (Arc1[idx2] + 1 < Arc1[idx4]))) {
							Obj1[idx4] = New1;
						Arc1[idx4] = Arc[idx2] + 1;
					}
					j += 1;
				}
			}
		}
	}

	if (Arc1[Obj1_len - 1] != number_Of_vehicle) {
		Result* res = (Result*)malloc(sizeof(Result));
		res->Obj1 = NULL;
		res->Obj2 = NULL;
		return res;
	}
	
	if (Obj1[Obj1_len - 1] > Obj1[Obj1_len - 2]) {
		for (int i = 0; i < Obj_len; i++) {
			int idx = 2 * i;
			Obj[i] = Obj1[idx];
			Arc[i] = Arc1[idx];
		}
	}

	else {
		for (int i = 0; i < Obj_len; i++) {
			int idx = 2 * i + 1;
			Obj[i] = Obj1[idx];
			Arc[i] = Arc1[idx];
		}
	}
	int Obj2;
	int temp_cost;
	for (int i = 1; i < Obj_len; i++) {
		if ((Arc[i] < Arc[i + 1]) && (Arc[i] == 1)) {
			Obj2 = Obj[i];
			temp_cost = Obj[i];
		}
		else if ((i == Obj_len - 1) && (Obj2 < Obj[i] - temp_cost)) {
			Obj2 = Obj[i];
		}
		else if ((Arc[i] < Arc[i + 1]) && (Obj2 < Obj[i] - temp_cost)) {
			Obj2 = Obj[i];
			temp_cost = Obj[i];
		}
	}
	Result* res = (Result*)malloc(sizeof(Result));
	res->Obj1 = Obj[Obj_len];
	res->Obj2 = Obj2;

	free(Arc1);
	free(Obj);
	free(Obj1);
	free(Arc);

	return res;

}

int linear_search(int arr[], int length, int target) {
	for (int i = 0; i < length; i++) {
		if (arr[i] == target) {
			return i; // 원하는 값의 인덱스 반환
		}
	}
	return -1; // 값을 찾지 못한 경우 -1 반환
}

int cost_(i, j, cost_demand) {
	if (i > j) {
		int temp = i;
		i = j;
		j = temp;
	}
	return cost_demand[i - 1][j - 1];
}

int demand_(i, j, cost_demand) {
	if (i < j) {
		int temp = i;
		i = j;
		j = temp;
	}
	return cost_demand[i - 1][j - 1];
}
int _inverse(int S, int length) {
	if (S < length){
		return S+length
	}
	else {
		return S-length
	}
}

int pareto_Dominance(S_cost1, S_cost2, N_cost1, N_cost2) {
	if ((S_cost1 == N_cost1) && (S_cost2 == N_cost2)){
		return -1
	}
	else if ((S_cost1 <= N_cost1) && (S_cost2 <= N_cost2)) {
		return 0
	}
	else if ((S_cost1 >= N_cost1) && (S_cost2 >= N_cost2)) {
		return 2
	}
	else {
		return 1
	}
}

KKfind_k(Archive_len) {

	int Archive_len = 10;
	
	int* k0;
	int* k1;
	int* k2;
	
	int* k0_cost;
	int* k1_cost;
	int* k2_cost;


	k0 = (int*)malloc(Archive_len * sizeof(int));
	k1 = (int*)malloc(Archive_len * sizeof(int));
	k2 = (int*)malloc(Archive_len * sizeof(int));

	k0_cost = (int*)malloc(Archive_len*2 * sizeof(int));
	k1_cost = (int*)malloc(Archive_len*2 * sizeof(int));
	k2_cost = (int*)malloc(Archive_len*2 * sizeof(int));


	for (int i = 0; i < Archive_len; i++) {
		k0[i] = -1;
		k1[i] = -1;
		k2[i] = -1;
		k0_cost[i] = -1;
		k0_cost[2*i] = -1;
		k1_cost[i] = -1;
		k1_cost[2*i] = -1;
		k2_cost[i] = -1;
		k2_cost[2*i] = -1;
	}

	for (int i = 0; i < Archive_len; i++) {
		int S_cost1, S_cost2, N_cost1, N_cost2;
		S_cost1 = Archive_cost[i][0];
		S_cost2 = Archive_cost[i][1];
		N_cost1 = new_pt_cost[0];
		N_cost2; = new_pt_cost[1];
		int k_dom = pareto_Dominance(S_cost1, S_cost2, N_cost1, N_cost2);
		if (k_dom == 0) {
			k0[i] = Archive[i];
			k0_cost[i] = Archive_cost[i][0];
			k0_cost[2*i] = Archive_cost[i][1];
		}
		else if(k_dom == 1) {
			k1[i] = Archive[i];
			k1_cost[i] = Archive_cost[i][0];
			k1_cost[2*i] = Archive_cost[i][1];
		}
		else if (k_dom == 2) {
			k2[i] = Archive[i];
			k2_cost[i] = Archive_cost[i][0];
			k2_cost[2 * i] = Archive_cost[i][1];
		}
	}
	KK res;
	k k_Archive;
	K k_cost;

	res.res1 = (int*)malloc(3*sizeof(int));
	res.res2 = (int*)malloc(3*sizeof(int));

	K k_Archive->k1 = k0;
	K k_Archive->k2 = k1;
	K k_Archive->k3 = k2;

	K k_cost->k1 = k0_cost;
	K k_cost->k2 = k1_cost;
	K k_cost->k3 = k2_cost;

	KK res->res1 = k_Archive;
	KK res->res2 = k_cost;

	return res;
}

int dom_calculator(a_cost1,a_cost2,b_cost1,b_cost2,R1,R2) {
	A = abs(a_cost1 - b_cost1) / R1;
	B = abs(a_cost2 - b_cost2) / R2;
	int result = A + B;
	return result;
}
domdel dominance_Delete(Archive, Archive_cost,Archive_Len) {
	for (int i = 0; i < Archive_Len - 1; i++) {
		for (int j = i + 1; j < Archive_Len; j++) {
			int dom = 0;
			//Archive cost 두배로 늘릴 계획
			dom = pareto_Dominance(Archive_cost[i], Archive_cost[j]);
			if (dom == 2){
				Archive[i] = -1;
				Archive_cost[i] = -1;
				return dominance_Delete(Archive, Archive,Archive_Len);
			}
			else if (dom == 0) {
				Archive[j] = -1;
				Archive_cost[j] = -1;
				return dominance_Delete(Archive, Archive_cost, Archive_Len);

			}
		}
	}
	domdel* res;
	res->Archive = (int*)malloc(Archive_Len * sizeof(int));
	res->Archive_cost = (int*)malloc(Archive_Len * sizeof(int));
	res->Archive = Archive;
	res->Archive_cost = Archive_cost;
	return res;
}