#include "DistributionSampler.h"
#include <cmath>
#include <iostream>
#include "Vec3D.h"
#include <vector>
#include "Particle.h"

float sine(float x){
    return std::sin(x);
}

float uniform(float x){
    return 1;
}

float plumberModelVel(float q){
    return q*q * sqrt(powf32(1-q*q, 7));
}


std::vector<float> DistributionSampler::sample(int nSamples, float min, float max){

    std::uniform_real_distribution<float> distrX(min, max);
    std::uniform_real_distribution<float> distrY(0, 1);

    std::vector<float> data;
    int generated = 0;
    while (generated < nSamples){

        auto x = distrX(generator_);
        auto y = distrY(generator_);

        if (y < pdf_(x)){

            data.push_back(x);
            generated++;

        }

    }

    return data;

}


/*
Sample points on a spherical surface centered at the origin.
Points are generated uniformly within a spherical volume,
normalised to unit length (projected onto the unit sphere),
and then projected onto the shell of radius r.
*/
std::vector<Vec3D> sampleSphereSurface(const int nSamples, const float r){

    DistributionSampler s(uniform);
    
    std::vector<Vec3D> samples;
    int generated = 0;
    while (generated < nSamples){

        // Generate x,y,z
        auto coords = s.sample(3, -r, r);        
        Vec3D v(coords.at(0), coords.at(1), coords.at(2));
        auto v_mag = Vec3D::mag(v);

        // Reject if mag too small to avoid flp errors
        if (v_mag < r && v_mag > 0.0001){
            samples.push_back((r/v_mag)*v);
            generated++;
        }

    }

    return samples;

}


/* Sample stars from the Plummer distribution */
std::vector<Particle> DistributionSampler::samplePlummerStars(int nStars, float R, float M){

    DistributionSampler s(uniform);
    
    auto temp = s.sample(nStars, 0, 1);
    std::vector<float> rs;

    for (auto t : temp)
    {
        auto t_ = std::pow(t, -2.0/3.0) - 1;
        auto t__ = std::pow(t_, -0.5);
        rs.push_back(t__);
    }

    std::vector<Vec3D> positions;
    for (auto &&r : rs)
    {
        positions.push_back(sampleSphereSurface(1, r).at(0));
    }
    
    DistributionSampler vSampler(plumberModelVel);
    auto qs = vSampler.sample(nStars, 0, 1);

    std::vector<float> vMags;
    for (size_t i = 0; i < qs.size(); i++)
    {
        auto temp = std::sqrt(2) * std::pow((1+rs.at(i)*rs.at(i)), -.25);
        vMags.push_back(qs.at(i)*temp);
    }

    std::vector<Vec3D> vs;
    for (auto &&v : vMags)
    {
        vs.push_back(sampleSphereSurface(1, v).at(0));
    }
    
    
    std::vector<Particle> particles;
    for (size_t i = 0; i < positions.size(); i++)
    {
        particles.push_back(Particle(positions.at(i), vs.at(i), 1.0/nStars));
    }
    
    for (auto &&p : particles)
    {
        p.pos = p.pos;
        p.vel = p.vel;
    }

    return particles;

}






// int main(){

//     // DistributionSampler sampler(uniform);

//     // sampler.sample(3, 0, 1);

//     // auto samples = sampler.getData();
//     // for (auto &&s : samples)
//     // {
//     //     std::cout << s << "\n";
//     // }
    
//     // std::cout << "\n";


//     auto points = sampleSphereSurface(2000, 10);
//     for (auto &&p : points)
//     {
//         std::cout << p << "\n";
//     }
    


//     return 0;
// }