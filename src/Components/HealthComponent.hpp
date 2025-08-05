#ifndef HEALTHCOMPONENT_HPP
#define HEALTHCOMPONENT_HPP

struct HealthComponent {
    int healthPercentage = 100;

    HealthComponent(int healthPercentage = 100) {
        this->healthPercentage = healthPercentage;
    };
};

#endif //HEALTHCOMPONENT_HPP
