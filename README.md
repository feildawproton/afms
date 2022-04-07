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

## Restricted Boltzmann Machine Training: Contrastive Divergence (single step)
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

1) 
  - take a training v
  - Compute the probabilities h
  - Sample h
    - In the architecture I use the sample of h is purposefully selected
   
![image](https://user-images.githubusercontent.com/56926839/162317754-4989fad0-2d62-4c9c-a243-0a9b8512836a.png)

2) Compute Positive Gradient

![image](https://user-images.githubusercontent.com/56926839/162317890-f9a054f8-f221-48e8-b432-e3fd29c243d5.png)

3) From h reconstruct v’, then resample h’

![image](https://user-images.githubusercontent.com/56926839/162317989-ad62738f-8d54-4437-96af-54439a409bfa.png)

4) Compute Negative Gradient

![image](https://user-images.githubusercontent.com/56926839/162318102-487852e0-8b50-4e3e-9313-6ed51e31ac8f.png)

5) Update W by ΔW where ϵ is the step size

![image](https://user-images.githubusercontent.com/56926839/162318159-f0b6207c-463f-43aa-a02a-0dc3211f9a4c.png)

6) Update a and b

![image](https://user-images.githubusercontent.com/56926839/162318529-8664f53b-3daa-4b64-9f4b-fe108180252f.png)

## Particular Application: Functional Movement Screening
- Multiple subclasses per movement
  - 12 individual movements
  - 48 total class categories
  - 104 total labels
- Depth video data

![image](https://user-images.githubusercontent.com/56926839/162320719-73dcd101-1500-4d8b-8f9f-665e7165bdb8.png)
![image](https://user-images.githubusercontent.com/56926839/162320771-5e815c33-541d-44b6-bb56-70c8a4ab5fa3.png)

## Chosen Architecture
- Convolution layer is a 4D hypercube
- The dimension CNNw corresponds to the vector of features
- We want activations have a binary distribution:

![image](https://user-images.githubusercontent.com/56926839/162321215-79d64502-5ecc-4dc7-9460-06a8dddeaf87.png)
![image](https://user-images.githubusercontent.com/56926839/162321304-ce46f053-278d-4833-ae4a-41c811bf5e0b.png)
![image](https://user-images.githubusercontent.com/56926839/162321388-4f4e6a7d-dc32-4e7a-82f6-29c27949c49e.png)







