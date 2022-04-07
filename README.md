# afms
An old (2016) project.  Part of it for school (msim 751) and part for work (afms python code).

The description below applies to the cpp project.  I also tried a 4D concolutional NN using tensorflow.

# Bayesian Data Analysis - Multipatameter Models
# Topic: Deep Belief Networks for Video Classification
2016 Nov 28
## Deep Belief Network
- Like a Bayesian network but we don’t assume prior knowledge of features, patterns, or causalities
- Constructed of restricted Boltzmann machine (RBM) layers
  - Each layer has many nodes and therefore many parameters
  - Once trained, these layers and their weight matrices will form our probabilistic model
![image](https://user-images.githubusercontent.com/56926839/162315889-33237a0b-1346-4002-80cb-f209a866ced4.png)

## Restricted Boltzmann Machine
- Visible Units are inputs
- Hidden Units are activated by the visible units via a weight matrix W with
![image](https://user-images.githubusercontent.com/56926839/162316330-224db8f8-418f-478a-a035-8c6a8ec7de30.png)
  - Where a and b are bias weights
- Visible units and hidden units are binary valued
- Layers are bipartite
  - No interconnections
![image](https://user-images.githubusercontent.com/56926839/162316694-e37b83dc-6481-4f3d-91b1-0e2b4414bbe8.png)

- The probability distribution over hidden and visible units depend on the activation function

![image](https://user-images.githubusercontent.com/56926839/162316988-d1d78ffd-44f8-4aa1-8018-5ad132070947.png)

- The conditional probability of v given h is

![image](https://user-images.githubusercontent.com/56926839/162317051-46688d8e-5982-4564-b8e1-4dbb8fb2ac79.png)

- With:

![image](https://user-images.githubusercontent.com/56926839/162317133-2d53cf0f-f419-420e-9989-a2b0b3c6a64d.png)




