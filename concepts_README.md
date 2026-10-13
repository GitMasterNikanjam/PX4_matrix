# Mathematical Concepts Behind PX4 Matrix

This guide introduces the mathematical ideas used by the PX4 Matrix library. It is theory-only: it contains no source code or programming instructions. For API details and usage examples, see the [guide index](README.md).

## 1. Vectors and vector spaces

A vector can represent a geometric quantity with magnitude and direction, or an ordered list of components. For example, a three-dimensional vector may represent position, velocity, or force. Vector addition and scalar multiplication are the basic operations of a vector space.

The Euclidean norm is the vector's length. Dividing a nonzero vector by its norm produces a unit vector: it retains direction while setting the length to one.

$$
\|\mathbf{x}\|_2 = \sqrt{x_1^2 + x_2^2 + \cdots + x_n^2},
\qquad
\hat{\mathbf{x}} = \frac{\mathbf{x}}{\|\mathbf{x}\|_2}.
$$

The zero vector cannot be normalized because its norm is zero.

The **dot product** measures alignment. For vectors \(\mathbf{a}\) and \(\mathbf{b}\),

$$
\mathbf{a} \cdot \mathbf{b}
= \sum_{i=1}^{n} a_i b_i
= \|\mathbf{a}\|_2\,\|\mathbf{b}\|_2\cos\theta.
$$

A positive result means the angle \(\theta\) is acute; zero means the vectors are perpendicular; a negative result means the angle is obtuse. In three dimensions, the **cross product** produces a vector perpendicular to both inputs. Its magnitude is the area of their parallelogram, and its direction follows the right-hand rule. In two dimensions, the corresponding scalar cross product gives signed area and rotation direction.

## 2. Matrices and linear transformations

A matrix represents a linear transformation. Each column describes where one coordinate basis vector is mapped. Multiplying a matrix by a vector forms a weighted sum of those columns:

$$
\mathbf{y} = A\mathbf{x}.
$$

This same notation represents a system of linear equations. The **transpose** \(A^T\) swaps rows and columns. A square matrix has an inverse \(A^{-1}\) when its transformation is reversible:

$$
A^{-1}A = AA^{-1} = I.
$$

A matrix is singular when its rows or columns are linearly dependent; it has no inverse. A matrix can also be nearly singular, in which case small input errors may cause large changes in the solution.

A matrix is **symmetric** when \(A=A^T\). A symmetric matrix is **positive definite** when every nonzero vector has positive quadratic form:

$$
\mathbf{x}^T A\mathbf{x} > 0
\quad\text{for every }\mathbf{x}\ne\mathbf{0}.
$$

Positive-definite matrices occur in covariance models and admit a Cholesky factorization:

$$
A = LL^T,
$$

where \(L\) is lower triangular. This factorization can make solving systems and related calculations more efficient.

## 3. Projections, least squares, and the pseudoinverse

An overdetermined linear system has more equations than unknowns and often has no exact solution. The **least-squares solution** chooses \(\mathbf{x}\) to minimize the squared residual:

$$
\mathbf{x}_{\mathrm{LS}}
= \underset{\mathbf{x}}{\operatorname{argmin}}\,
\|A\mathbf{x}-\mathbf{b}\|_2^2.
$$

Geometrically, \(A\mathbf{x}_{\mathrm{LS}}\) is the projection of \(\mathbf{b}\) onto the column space of \(A\). The residual is perpendicular to that space. When the columns of \(A\) are independent, this condition is often written as the normal equations:

$$
A^T A\mathbf{x}_{\mathrm{LS}} = A^T\mathbf{b}.
$$

The **QR factorization** writes a matrix as \(A=QR\), where the columns of \(Q\) are orthonormal and \(R\) is upper triangular. Orthogonality makes QR useful for least-squares problems. Householder reflections are a way to construct this factorization while maintaining good numerical behavior.

The **Moore–Penrose pseudoinverse** \(A^+\) extends the idea of an inverse to rectangular and rank-deficient matrices. It provides a least-squares solution for an overdetermined system and, among solutions to an underdetermined system, selects the one with minimum Euclidean norm:

$$
\mathbf{x}_{\min\text{-norm}} = A^+\mathbf{b}.
$$

One useful full-column-rank identity is \(A^+=(A^T A)^{-1}A^T\). Forming \(A^T A\), however, can worsen numerical conditioning: roughly speaking, its condition number is the square of that of \(A\). Nearly dependent columns therefore require particular care.

## 4. Rotations in two and three dimensions

A rotation preserves lengths and angles. Its matrix \(R\) is orthogonal and has determinant \(+1\):

$$
R^T R = I,
\qquad \det(R)=1.
$$

Consequently, \(R^{-1}=R^T\). Combining rotations corresponds to matrix multiplication, and in three dimensions the order of multiplication matters.

In two dimensions, one angle specifies a rotation. In three dimensions, the same orientation can be represented in several ways:

- **Euler angles** describe a sequence of rotations about specified axes. The axis sequence is part of the definition. For the 3-2-1 convention, pitch at \(\pm 90^\circ\) creates a singularity: roll and yaw cannot be independently recovered. This is called gimbal lock.
- **Axis–angle** describes a unit axis \(\hat{\mathbf{u}}\) and an angle \(\theta\). The rotation vector is \(\boldsymbol{\phi}=\theta\hat{\mathbf{u}}\). At zero rotation, the axis is not unique.
- A **unit quaternion** uses four components to represent a rotation without Euler-angle singularities. The quaternions \(q\) and \(-q\) represent the same physical orientation. A quaternion represents a pure rotation when its norm is one.
- A **direction cosine matrix (DCM)** directly maps coordinates between frames. It has nine components and is geometrically explicit, though numerical rounding can gradually erode its orthogonality.

These representations describe the same rotation geometry, but trade off compactness, interpretability, uniqueness, and numerical behavior. Rotations also do not form an ordinary flat Euclidean space, so interpolating their components as if they were points can produce misleading paths.

## 5. Derivatives, Jacobians, and dual numbers

A derivative describes the local rate of change of a function. For a vector-valued function of a vector, the **Jacobian** collects all partial derivatives. Near a point \(\mathbf{x}\), it gives the first-order approximation

$$
\mathbf{f}(\mathbf{x}+\Delta\mathbf{x})
\approx \mathbf{f}(\mathbf{x}) + J(\mathbf{x})\,\Delta\mathbf{x}.
$$

This linearization is useful for motion analysis, state estimation, and uncertainty propagation.

**Dual numbers** extend ordinary numbers with a value part and an infinitesimal part. They obey \(\varepsilon^2=0\). Evaluating a differentiable function with \(x+\varepsilon\) propagates its derivative through the arithmetic:

$$
f(x+\varepsilon) = f(x) + f'(x)\varepsilon.
$$

This is the basis of forward-mode automatic differentiation. Unlike finite differences, it does not estimate a derivative by subtracting two nearby function values, so it avoids that source of cancellation error.

## 6. Ordinary differential equations and RK4

An ordinary differential equation (ODE) relates a quantity to its rate of change. An initial-value problem specifies both the differential equation and the starting state:

$$
\frac{d\mathbf{x}}{dt} = \mathbf{f}(t,\mathbf{x}),
\qquad \mathbf{x}(t_0)=\mathbf{x}_0.
$$

Numerical integration advances an approximate solution over small time intervals. The classical fourth-order Runge–Kutta method (RK4) samples the slope four times per interval: at the start, at two midpoint estimates, and at the end. A weighted average of these slopes advances the state:

$$
\begin{aligned}
k_1 &= f(t_n,x_n),\\
k_2 &= f\!\left(t_n+\frac{h}{2},x_n+\frac{h k_1}{2}\right),\\
k_3 &= f\!\left(t_n+\frac{h}{2},x_n+\frac{h k_2}{2}\right),\\
k_4 &= f(t_n+h,x_n+h k_3),\\[2pt]
x_{n+1} &= x_n + \frac{h}{6}(k_1+2k_2+2k_3+k_4).
\end{aligned}
$$

Here \(h\) is the time step. For a sufficiently smooth solution, RK4 agrees with the Taylor expansion through terms of order \(h^4\), giving global error of order \(h^4\) over a fixed interval under the usual assumptions. It does not explicitly calculate higher derivatives, and it does not automatically adjust its step size. A step that is too large, especially for a stiff or unstable system, can still produce poor results.

## 7. Uncertainty, covariance, and the Kalman correction

The **variance** measures the spread of a random quantity around its mean. **Covariance** describes how two quantities vary together. A covariance matrix \(P\) collects both kinds of information: its diagonal contains variances and its off-diagonal entries contain cross-covariances. A valid covariance matrix is symmetric and positive semidefinite:

$$
P=P^T,
\qquad \mathbf{z}^T P\mathbf{z}\ge 0
\quad\text{for all }\mathbf{z}.
$$

A Kalman filter combines a prior state estimate with a new measurement. The **innovation** is the difference between the observed measurement and the measurement predicted from the prior:

$$
\boldsymbol{\nu}=\mathbf{z}-H\hat{\mathbf{x}}^{-}.
$$

Its covariance \(S\) describes the expected uncertainty in that difference. The Kalman gain balances prior uncertainty against measurement uncertainty:

$$
K=P^{-}H^T(HP^{-}H^T+R)^{-1},
\qquad
\hat{\mathbf{x}}^{+}=\hat{\mathbf{x}}^{-}+K\boldsymbol{\nu}.
$$

Here \(P^{-}\) is prior covariance, \(R\) is measurement-noise covariance, and superscripts \(-\) and \(+\) denote before and after measurement correction. A more reliable measurement (smaller \(R\)) generally receives more weight. The simple covariance update is

$$
P^{+}=(I-KH)P^{-}.
$$

In finite-precision arithmetic, this simplified expression may lose symmetry or positive semidefiniteness through rounding. That matters because those properties give covariance its interpretation as uncertainty.

The **Mahalanobis distance** measures an innovation relative to its covariance rather than by raw Euclidean size:

$$
d^2 = \boldsymbol{\nu}^T S^{-1}\boldsymbol{\nu}.
$$

If the innovation is Gaussian with the assumed covariance, \(d^2\) follows a chi-squared distribution with degrees of freedom equal to the measurement dimension. This gives a statistical basis for identifying measurements that are inconsistent with the model.

## 8. Matrix exponential

The matrix exponential extends the scalar exponential to square matrices through a power series:

$$
e^A = I + A + \frac{A^2}{2!} + \frac{A^3}{3!} + \cdots
= \sum_{k=0}^{\infty}\frac{A^k}{k!}.
$$

It appears in the exact solution of linear continuous-time systems. If \(\dot{\mathbf{x}}=A\mathbf{x}\) with constant \(A\), then \(\mathbf{x}(t)=e^{At}\mathbf{x}(0)\). A truncated polynomial can approximate the exponential for suitable matrices and arguments; numerical methods often use transformations and scaling to maintain accuracy.

## 9. Floating-point arithmetic and numerical stability

Computers represent most real numbers with a finite number of bits. Arithmetic is therefore rounded, and familiar real-number identities may no longer hold exactly. **Machine epsilon** describes the spacing near one between adjacent representable numbers; it is not a universal tolerance for every magnitude or problem.

The **condition number** measures how sensitive a mathematical problem is to small input changes. **Numerical stability** describes how an algorithm handles rounding and other small errors. A well-designed algorithm cannot remove sensitivity inherent in an ill-conditioned problem, and a poorly behaved algorithm can spoil an otherwise well-conditioned problem. This distinction is important when interpreting matrix inverses, numerical rank, normalization, and least-squares results.

## Suggested learning sequence

1. Vectors, dot products, matrices, and linear systems.
2. Orthogonal projection, least squares, QR, and the pseudoinverse.
3. Three-dimensional rotations and their alternative representations.
4. Derivatives, dual numbers, and ordinary differential equations.
5. Covariance, Kalman estimation, and numerical stability.

## Further learning

### Linear algebra and least squares

- [MIT OpenCourseWare: Projection Matrices and Least Squares](https://ocw.mit.edu/courses/18-06sc-linear-algebra-fall-2011/pages/least-squares-determinants-and-eigenvalues/projection-matrices-and-least-squares/) — geometric intuition for projections and least squares.
- [MIT OpenCourseWare: Four Ways to Solve Least-Squares Problems](https://ocw.mit.edu/courses/18-065-matrix-methods-in-data-analysis-signal-processing-and-machine-learning-spring-2018/resources/lecture-9-four-ways-to-solve-least-squares-problems/) — compares normal equations, QR, and the pseudoinverse.
- [Linear Algebra for Robotics, University of Illinois](https://motion.cs.illinois.edu/RoboticSystems/LinearAlgebra.html) — linear algebra concepts in a robotics context.

### Three-dimensional rotations

- [3D Rotations, University of Illinois](https://motion.cs.illinois.edu/RoboticSystems/3DRotations.html) — rotation matrices, Euler angles, axis–angle, and quaternions.
- [Robotics Foundations, Universitat Politècnica de Catalunya](https://sir.upc.edu/projects/kinematics_dynamics_control_theory/foundations/index.html) — university notes on rotation representations.

### Derivatives and dual numbers

- [MIT OpenCourseWare: Automatic Differentiation via Dual Numbers](https://ocw.mit.edu/courses/18-s096-matrix-calculus-for-machine-learning-and-beyond-january-iap-2023/resources/mit18_s096iap23_lec08/) — forward-mode automatic differentiation theory.
- [Automatic Differentiation, Cornell University](https://www.cs.cornell.edu/~bindel/nmds/01-Fund1d/04-AutoDiff.html) — forward-mode differentiation and dual numbers.

### Differential equations and numerical integration

- [Differential Equations and Integrals, University of Minnesota Duluth](https://www.d.umn.edu/~mhampton/NA/ODEInt/) — numerical methods for initial-value problems, including RK4.
- [Runge–Kutta Methods, University of Texas at Austin](https://farside.ph.utexas.edu/teaching/329/lectures/node35.html) — an introduction to Runge–Kutta methods.

### Kalman filtering and uncertainty

- [MIT OpenCourseWare: Identification, Estimation, and Learning, Lecture 5](https://ocw.mit.edu/courses/2-160-identification-estimation-and-learning-spring-2006/resources/lecture_5/) — state-space estimation and Kalman filtering.
- [MIT OpenCourseWare: Algorithms for Inference, Lecture 13](https://ocw.mit.edu/courses/6-438-algorithms-for-inference-fall-2014/resources/mit6_438f14_lec13/) — Kalman filtering from a probabilistic inference perspective.
- [Oxford Estimation Lecture Notes](https://www.robots.ox.ac.uk/~ian/Teaching/Estimation/LectureNotes2.pdf) — innovation covariance and chi-squared consistency tests.

### Numerical computation and precision

- [Floating-Point Representation, University of Illinois](https://courses.physics.illinois.edu/cs357/sp2021/notes/ref-4-fp.html) — floating-point numbers and machine epsilon.
- [Numerical Linear Algebra for Signal Processing, Technical University of Munich](https://www.ce.cit.tum.de/en/msv/courses/master-lectures/numerical-linear-algebra-for-signal-processing/) — conditioning, stability, and matrix factorizations.
- [Numerical Algorithms, IIT Delhi](https://www.cse.iitd.ac.in/~narain/courses/2102-col726/) — course topics include conditioning, QR, Cholesky, and floating-point arithmetic.

## English-language video lessons

These videos complement the written references above. The mathematical guide remains code-free; a few video series continue into implementation after their theory sections.

### Linear algebra and least squares

- [MIT OpenCourseWare: Multiplication and Inverse Matrices](https://www.youtube.com/watch?v=FX4C-JpTFgY) — matrix multiplication, inverses, and their geometric meaning.
- [MIT OpenCourseWare: Projection Matrices and Least Squares](https://www.youtube.com/watch?v=osh80YCg_GM) — orthogonal projection and least-squares fitting.

### Three-dimensional rotations

- [3Blue1Brown: Quaternions and 3D Rotation, Explained Interactively](https://www.youtube.com/watch?v=zjMuIxRvygQ) — an intuitive visual explanation of quaternion rotations.
- [3Blue1Brown: Visualizing Quaternions](https://www.youtube.com/watch?v=d4EgbgTm0Bg) — builds intuition for quaternions as four-dimensional numbers.

### Derivatives and dual numbers

- [MIT OpenCourseWare: Forward Automatic Differentiation via Dual Numbers](https://www.youtube.com/watch?v=5F6roh4pmJU) — explains how dual numbers carry values and derivatives through a calculation.

### Differential equations and RK4

- [Numerical Methods Guy: Runge–Kutta Fourth-Order Method Formulas](https://www.youtube.com/watch?v=hGN54bkE8Ac) — a concise lesson on the four slope estimates and weighted update.
- [Holistic Numerical Methods: Fourth-Order Runge–Kutta Video Lesson](https://mathforcollege.com/nm/mobile/videos/08ode/Runge4th.html) — a concise lesson focused on RK4 formulas and intuition.

### Kalman filtering

- [MathWorks: Understanding Kalman Filters video series](https://www.mathworks.com/videos/series/understanding-kalman-filters.html) — start with “Why Use Kalman Filters?”, “State Observers”, “An Optimal State Estimator”, and “An Optimal State Estimator Algorithm” for the core ideas.
- [CLEAR Lab: Conditional Expectation and Covariance](https://www.youtube.com/watch?v=RhNCpfbIhXc) — probability and covariance concepts that prepare for the Kalman-filter derivation.

### Floating-point arithmetic

- [Holistic Numerical Methods: IEEE-754 Single-Precision Representation](https://nm.mathforcollege.com/chapter-01-05-ieee-754-single-precision-representation-part-1-of-2/) — video lessons on binary representation, machine epsilon, and precision limits.
- [Floating-Point Numbers and Machine Epsilon](https://www.youtube.com/watch?v=N_eFw8MHUcw) — explains rounding, machine epsilon, and loss of significance.
